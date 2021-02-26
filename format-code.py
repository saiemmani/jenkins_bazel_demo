import os 
import argparse

# python3 format-code..py -r ~/iw.hub/

# Set the current working directory for scanning c/c++ sources (including 
# header files) and apply the clang formatting 

# and "-i" is in-place editing 
command = """
find {}  | grep 'cpp\|hpp\|cc' | xargs -I{} sh -c 'mkdir -p ./linting && export clang="./linting/$(basename $1)" && export output="./linting/$(basename $1).patch" && clang-format $1 > $clang && diff -u $1 $clang > $output' -- {}
"""
def main():
    parser = argparse.ArgumentParser(description="Str 4 Linting")
    parser.add_argument("-r",
                        "--root_directory",
                        type=str,
                        help="Root directory of the folders subject to linting",
                        required=False)

    args = parser.parse_args()
    root_dir = args.root_directory
    os.system(command.format(root_dir,"{}","{}"))

if __name__ == '__main__':
    main()