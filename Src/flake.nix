{
  description = "A flake for building, testing and benchmarking bit-reversal algorithms and plotting the results";

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
        rec {
          default =
            pkgs.mkShell.override
              {
                # Override stdenv in order to change compiler:
                stdenv = pkgs.clangStdenv;
                # LD_LIBRARY_PATH = "${pkgs.stdenv.cc.cc.lib}/lib";
              }
              {
                packages = [
                  pkgs.cudatoolkit
                  pkgs.cudaPackages.cudnn
                  pkgs.cudaPackages.cuda_cudart
                  pkgs.cudaPackages.cuda_nvvp
                  pkgs.bear
                  pkgs.typst
                  pkgs.llvmPackages.clang-tools
                  pkgs.uv
                  pkgs.valgrind
                  pkgs.biber
                  pkgs.jre8
                  pkgs.perf
                  pkgs.inotify-tools
                  pkgs.zig

                ]
                ++ (if pkgs.system == "aarch64-darwin" then [ ] else [ pkgs.gdb ]);

                shellHook = ''
                    # Required for clangd to work
                    export GCC_DIR="${pkgs.gcc.cc}"
                    export GCC_VER="${pkgs.gcc.version}"
                    export CPLUS_INCLUDE_PATH="$GCC_DIR/include/c++/$GCC_VER:$GCC_DIR/include/c++/$GCC_VER/x86_64-unknown-linux-gnu:$CPLUS_INCLUDE_PATH"
                    export NIXPKGS_ALLOW_UNFREE=1
                    export NIX_ENFORCE_NO_NATIVE=0

                    # CUDA
                    # Proper library path construction using makeLibraryPath
                    export CUDA_PATH=${pkgs.cudatoolkit}
                    export LD_LIBRARY_PATH=${
                      pkgs.lib.makeLibraryPath [
                        "/run/opengl-driver" # Needed to find libGL.so
                        pkgs.cudatoolkit
                        pkgs.cudaPackages.cudnn
                      ]
                    }:"${pkgs.stdenv.cc.cc.lib}/lib":$LD_LIBRARY_PATH
                    # Set LIBRARY_PATH to help the linker find the CUDA static libraries
                    export LIBRARY_PATH=${
                      pkgs.lib.makeLibraryPath [
                        pkgs.cudatoolkit
                      ]
                    }:$LIBRARY_PATH
                    # Check if file required by clangd exists
                  # '';
              };
        }
      );
    };
}
