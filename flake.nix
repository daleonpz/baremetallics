{
    description = "Flake for Baremetallics";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
    };

    outputs = { self, nixpkgs }:
    
    let 
        system = "x86_64-linux";
        pkgs = import nixpkgs { system = system; };
    in
    {
        devShells.${system}.default = pkgs.mkShell {
            name = "baremetallics";
            buildInputs = with pkgs; [
              ruby_3_4
              gcc
              gnumake
              tree
            ];

            shellHook = ''
              echo "Welcome to Baremetallics!"
              export HOME=$(pwd)

              gem install bundler
              gem install jekyll
              bundle install
                    '';
        };
    };
}
