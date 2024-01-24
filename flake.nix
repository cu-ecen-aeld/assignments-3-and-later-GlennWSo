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
        ];

        dev_tools = with pkgs; [
          clang-tools
          cmake
          ruby
        ];
      in
      with cross_pkgs;
      {
        devShells.default = mkShell {
          name = "advanded embedded linux"; 
          buildInputs = buildDeps;
          nativeBuildInputs= dev_tools;
          shellHook = ''
            echo hi
          '';
        };
      }
    );
}
