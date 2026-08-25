{
  description = "awt - Ultimate Mpvpaper Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "awt";
            version = "1.0.0";
            src = ./.;

            nativeBuildInputs = [ pkgs.makeWrapper ];

            buildPhase = ''
              runHook preBuild
              $CC -O2 main.c -o awt
              runHook postBuild
            '';

            installPhase = ''
              runHook preInstall
              mkdir -p $out/bin
              cp awt $out/bin/
              runHook postInstall
            '';

            postFixup = ''
              wrapProgram $out/bin/awt \
                --prefix PATH : ${pkgs.lib.makeBinPath [ 
                  pkgs.coreutils 
                  pkgs.procps 
                  pkgs.mpvpaper 
                ]}
            '';

            meta = with pkgs.lib; {
              description = "Ultimate Mpvpaper Engine";
              mainProgram = "awt";
              platforms = platforms.linux;
            };
          };
        });

      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            packages = with pkgs; [
              gcc
              coreutils
              procps
              mpvpaper
            ];
          };
        });

      nixosModules.default = { config, lib, pkgs, ... }:
        let
          cfg = config.programs.awt;
        in
        {
          options.programs.awt = {
            enable = lib.mkEnableOption "awt";
            package = lib.mkOption {
              type = lib.types.package;
              default = self.packages.${pkgs.system}.default;
              description = "awt package";
            };
          };

          config = lib.mkIf cfg.enable {
            environment.systemPackages = [ cfg.package ];
          };
        };
    };
}
