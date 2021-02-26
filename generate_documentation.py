import os
import argparse
import re
from os import path
import shutil

# Documentation template
documentation_template = """
## {0}

#### Files 
[{1}]({2}) 

#### Description 
{3}
#### Incoming messages 
{4}
#### Outgoing messages 
{5}
#### Parameters 
{6}
--- 
"""

# regex pattern that checks whether a file is registered as a codelet.
regex_register = r'(?<=(?<!\/\/)ISAAC_ALICE_REGISTER_CODELET\()str::*.+(?=\))'

# regex pattern that finds all the comments that come right before the <class> token
regex_class_description = r'(((((\/\/.*[\r \n]*)+^([ \r\n]*class))))|(((\/\*.*\*\/[\r\n]*)+^([ \r\n]*class)))|(((' \
                          r'\/\*+((([^\*])+)|([\*]+(?!\/)))[*]+\/[\r\n]*)^([\r\n]*class))))|^([ \r\n]*class) '

# regex pattern that will find all the comments before an ISAAC_PARAM declaration + the ISAAC_PARAM declaration itself
regex_param_match_comment = r'(((((\/\/(?![ ]*TODO[ ]*).*[\r \n]*)+ISAAC_PARAM\(.*?\)*.*\))))|(((\/\*.*\*\/[\r ' \
                            r'\n]*)+ISAAC_PARAM\(.*?\)*.*\)))|(((\/\*+((([^\*])+)|([\*]+(?!\/)))[*]+\/[\r\n ' \
                            r']*)ISAAC_PARAM\(.*?\)*.*\))))|(ISAAC_PARAM\(.*?\)*.*\))'

# regex pattern that will find all the comments before an ISAAC_PROTO_RX declaration + the ISAAC_PROTO_RX declaration
# itself
regex_rx_match_comment = r'(((((\/\/(?![ ]*TODO[ ]*).*[\r \n]*)+ISAAC_PROTO_RX\(.*?\)*.*\))))|(((\/\*.*\*\/[\r ' \
                         r'\n]*)+ISAAC_PROTO_RX\(.*?\)*.*\)))|(((\/\*+((([^\*])+)|([\*]+(?!\/)))[*]+\/[\r\n ' \
                         r']*)ISAAC_PROTO_RX\(.*?\)*.*\))))|(ISAAC_PROTO_RX\(.*?\)*.*\))'

# regex pattern that will find all the comments before an ISAAC_PROTO_TX declaration + the ISAAC_PROTO_TX declaration
# itself
regex_tx_match_comment = r'(((((\/\/(?![ ]*TODO[ ]*).*[\r \n]*)+ISAAC_PROTO_TX\(.*?\)*.*\))))|(((\/\*.*\*\/[\r ' \
                         r'\n]*)+ISAAC_PROTO_TX\(.*?\)*.*\)))|(((\/\*+((([^\*])+)|([\*]+(?!\/)))[*]+\/[\r\n ' \
                         r']*)ISAAC_PROTO_TX\(.*?\)*.*\))))|(ISAAC_PROTO_TX\(.*?\)*.*\))'

# remove comment symbols from matched string
regex_clean = r'(\/\/)|(\/\*)|(\*\/)'


def parse_arguments(text_data, regex):
    """
    Attributes
    ----------
    text_data : str
        A string containing a file's content
    regex : str
        A string containing the regex pattern
    """

    # find the regex matches
    data = re.finditer(regex, text_data, flags=re.MULTILINE)
    # Declare section title
    output = ""
    for elt in data:
        text = elt.group(0)
        # Eliminate the comments from the matched regex: //this is a test comment -> this is a test comment
        text = re.sub(regex_clean, '', text)
        # Remove the extra stars generate by multi line comments
        dirty_tokens = text.replace("\n*", "\n").split("\n")
        # Remove extra list elements caused by empty lines
        token_list = [line for line in dirty_tokens if line.strip() != ""]
        # Separate the element from the last match, in other words separate the comments from the C++ syntax
        _description = " ".join((token_list[0:len(token_list) - 1]))
        # Remove empty white spaces from the string
        _description = " ".join(_description.split())
        # When regex == regex_class_description, it means we are parsing the class description, no need to scan for
        # arguments. Return here.
        if regex == regex_class_description:
            output = output + "{} \n\n".format(_description)
            return output

        # Parse the arguments from the last element of the token list and clean it.
        args = token_list[len(token_list) - 1]
        args = args.replace(" ", "")

        # generate the list of arguments ISAAC_PARAM(x,y,z) => (x,y,z)
        args_start_index = args.find('(')
        args = args[args_start_index + 1:len(args) - 1].split(",")
        if len(args) == 2:
            (_name, _type) = args[1], args[0]
            output = output + "- {} [`{}`]: {}\n\n".format(_name, _type, _description)
        else:
            (_name, _type, _default) = args[1], args[0], ", ".join(args[2:])
            output = output + "- {} [`{}`][default={}]: {}\n\n".format(_name, _type, _default, _description)
    return output


def generate_tx_data(text_data):
    """
    Generates the ISAAC_PROTO_TX documentation using the comments written before it.
    Attempts to apply the same format as the ISAAC SDK.
    
    """
    return parse_arguments(text_data, regex_tx_match_comment)


def generate_rx_data(text_data):
    """
    Generates the ISAAC_PROTO_RX documentation using the comments written before it.
    Attempts to apply the same format as the ISAAC SDK.
    
    """
    return parse_arguments(text_data, regex_rx_match_comment)


def generate_param_data(text_data):
    """
    Generates the ISAAC_PARAM documentation using the comments written before it.
    Attempts to apply the same format as the ISAAC SDK.

    """
    return parse_arguments(text_data, regex_param_match_comment)


def generate_class_description(text_data):
    """
    Generate component description by combining all the comments before the class declaration token <class>
    """
    return parse_arguments(text_data, regex_class_description)


def get_class_registration(text_data):
    """
    Checks whether the source file is a registered codelet. If Yes Generate Documentation. If Not Skip
    """
    return re.findall(regex_register, text_data)


def main():
    parser = argparse.ArgumentParser(description="Str 4 README Generator")
    parser.add_argument("-r",
                        "--root_directory",
                        type=str,
                        help="Root directory of the folders containing the components header files",
                        required=True)

    parser.add_argument("-o",
                        "--output_folder",
                        type=str,
                        help="Output folder which will contain all the READMEs. if not specified each README goes to "
                             "its rightful place",
                        required=False)

    args = parser.parse_args()
    root_dir = args.root_directory
    # generates file names in the directory tree by walking the tree top-down
    if args.output_folder:
        if not path.isdir(args.output_folder):
            os.mkdir(args.output_folder)
            print("Target directory not found. Creating ...")
        else:
            print("Target directory already exists. Overwriting ...")
            shutil.rmtree(args.output_folder)
            os.mkdir(args.output_folder)

    for root, subdirs, files in os.walk(root_dir):
        f = None
        # String containing the content of 1 Readme file
        readme_output = ""
        # If target directory is not found, create it
        # Flag that signals if a readme file has already been opened for writing
        created_flag = False
        for file in files:
            # loop over all header files
            if file[-3:] == 'hpp':
                with open(os.path.join(root, file), 'r') as source_file:
                    # extract source code
                    source_code = source_file.read()
                    # check if the hpp qualifies and that the file hasn't been opened yet
                    class_registration = get_class_registration(source_code)
                    if class_registration:
                        if not created_flag:
                            if args.output_folder:
                                f = open(args.output_folder + "/" + str(file[:-4]) + "_README.md", "w")
                            else:
                                f = open(root + "/README.md", "w")
                            created_flag = True
                        file_path_list = root.split("/")
                        # get_class_registration(source_code) returns a list of one element representing the
                        # registration name
                        name = str(class_registration[0].replace("::", "."))
                        hpp_file = file  # Add hpp file name to README
                        reference = "../" + "/".join(file_path_list[-3:]) + "/" + file  # Add reference to hpp file name
                        description = generate_class_description(source_code)  # Add class description to README
                        rx = generate_rx_data(source_code)  # Add incoming messages to README
                        tx = generate_tx_data(source_code)  # Add outgoing messages to README
                        param = generate_param_data(source_code)  # Add parameters to README
                        readme_output += documentation_template.format(name, hpp_file, reference, description, rx, tx,
                                                                       param)
                    else:
                        continue
            else:
                continue
        if f:
            f.write(readme_output)
            f.close()


if __name__ == '__main__':
    main()
