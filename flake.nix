{
  description = "A devShell that can run three-d examples";

  inputs = {
    nixpkgs.url      = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url  = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs,  flake-utils}:
    flake-utils.lib.eachDefaultSystem (system:
      let
        cross_pkgs = import nixpkgs {
          inherit system;
          crossSystem.config = "aarch64-unknown-linux-gnu";
        };

        pkgs = import nixpkgs {
          inherit system;
        };

        buildDeps = with cross_pkgs; [
          cmake
          ruby
          # derp
          # getopt
          # flex
          # bison
          # gcc
          # gnumake
          # bc
          # pkg-config
          # binutils
        ];

        dev_tools = with pkgs; [
          clang-tools
          cmake
          ruby
          pkg-config
          ncurses
          flex
          bison
        ];

        linux = pkgs.linux.overrideAttrs (o: {nativeBuildInputs=o.nativeBuildInputs ++ [ pkgs.pkg-config pkgs.ncurses ];});

        
      in
      # with cross_pkgs;
      {
        devShells.default = cross_pkgs.mkShell {
          name = "pure"; 
          buildInputs = buildDeps;
          nativeBuildInputs= dev_tools;
          shellHook = ''
            echo hi
          '';
        };

        packages.menu = linux;
      }
    );
}
