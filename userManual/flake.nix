{
  description = "A flake to manage the typst compilation environment";

  inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0.1.*.tar.gz";

  outputs =
    { self, nixpkgs }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      forEachSupportedSystem =
        f:
        nixpkgs.lib.genAttrs supportedSystems (
          system:
          f {
            pkgs = import nixpkgs {
              inherit system;
              config = {
                allowUnfree = true;
              };
            };
          }
        );
    in
    {
      devShells = forEachSupportedSystem (

        { pkgs }:
        {
          fontsConf = pkgs.makeFontsConf {
            fontDirectories = [
              # your needed fonts here, e.g.:
              pkgs.courier-prime
              pkgs.liberation-sans-narrow
              pkgs.liberation_ttf_v1
              pkgs.libertinus
              pkgs.inconsolata
            ];
          };
          default =
            pkgs.mkShell.override
              {
                # Override stdenv in order to change compiler:
                stdenv = pkgs.clangStdenv;
                # LD_LIBRARY_PATH = "${pkgs.stdenv.cc.cc.lib}/lib";
              }
              {

                packages = [
                  pkgs.typst
                  pkgs.biber
                  pkgs.courier-prime
                  pkgs.typstPackages.fontawesome
                  pkgs.typstPackages.cetz
                ];
              };
        }
      );
    };
}
