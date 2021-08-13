import os
import sys
import subprocess
import os.path
import fileinput
import re
import collections
import copy
from datetime import datetime, date
import pytz

import Globals

EX_OK = getattr(os, "EX_OK", 0)


###########################################################################
# Object Storing Class
###########################################################################
class Variable:
    def __init__(self):
        self.allowed_values_ = ""
        self.name_ = ""
        self.type_ = ""
        self.description_ = ""
        self.value_ = ""
        self.default_ = "No default"
        self.lower_bound_ = ""
        self.upper_bound_ = ""
        self.note_ = ""


class Class:
    def __init__(self):
        self.name_ = "No Name"
        self.parent_name_ = ""
        self.variables_ = {}
        self.estimables_ = {}
        self.addressables_ = {}
        self.child_classes_ = {}
        self.variable_order_ = []

# Variables we want to use
type_aliases_ = {}  # Type Aliases convert C++ types to English text
translations_ = {}  # Translations stores all of our PARAM_X and the English text
parent_class_ = Class()  # Hold our top most parent class (e.g niwa::Process)

# Regular Expressions used later
reCapitalLetters = '(,|>|\(|\)|{|\}|;)(?=([^\"]*\"[^\"]*\")*[^\"]*$)'
reReplaceUnderscores = '(?<=[a-z])(?=[A-Z])'
reRemoveU = '(?:^d+)*u$'

###########################################################################
# Define defaults
###########################################################################
class Documentation:
    # Methods
    def __init__(self):
        print('--> Starting Documentation Builder')
        type_aliases_['double'] = 'Real number'
        type_aliases_['Double'] = 'Real number (estimable)'
        type_aliases_['unsigned'] = 'Non-negative integer'
        type_aliases_['bool'] = 'Boolean'
        type_aliases_['int'] = 'Integer'
        type_aliases_['string'] = 'String'
        type_aliases_['vector<double>'] = 'Vector of real numbers'
        type_aliases_['vector<Double>'] = 'Vector of real numbers (estimable)'
        type_aliases_['vector<unsigned>'] = 'Vector of non-negative integers'
        type_aliases_['vector<bool>'] = 'Vector of booleans (true/false)'
        type_aliases_['vector<int>'] = 'Vector of integers'
        type_aliases_['vector<string>'] = 'Vector of strings'
        type_aliases_['map<string, vector<string>>'] = 'Matrix of strings'
        type_aliases_['map<string, vector<double>>'] = 'Vector of real numbers'
        type_aliases_['map<string, vector<Double>>'] = 'Vector of real numbers (estimable)'
        type_aliases_['parameters::table*'] = 'Real number'

    ###########################################################################
    # Start the documentation builder
    ###########################################################################
    def start(self):
        # Figure out if we have LaTeX in our path, if we do, then we want to
        # build the PDF documentation as well
        if Globals.latex_path_ != "":
            self.build_latex_ = True
        else:
            return Globals.PrintError("LaTeX is not installed or cannot be detected properly. Please ensure LaTeX tools are in the system path")

        # Load our translations from the Translations.h file so we can convert
        # PARAM_XXX into English
        if not self.load_translations():
            return False

        if not ClassLoader().Run():
            return False

        return Latex().Build()

    ###########################################################################
    # Load the translations from our translation file. This information gets
    # stored in a map so it can be used for converting the PARAM_X names in to
    # English when we print the details to the file
    ###########################################################################
    def load_translations(self):
        print('--> Loading translations')
        file = fileinput.FileInput('../CASAL2/source/Translations/English_UK.h')
        if not file:
            return Globals.PrintError('Failed to open the English_UK.h for translation loading')
        for line in file:
            if not line.startswith('#define'):
                continue
            pieces = line.split()
            if len(pieces) < 3 or not pieces[1].startswith('PARAM'):
                continue
            lookup = pieces[1]
            # Simplify PARAM_XXX names, remove quotes and replace underscores
            value = pieces[2].replace('"', '').lstrip().rstrip().replace('_', '\_')
            translations_[lookup] = value

        print('    Loaded ' + str(len(translations_)) + ' translation values')
        return True


###########################################################################
# Read and load Casal2 source files to extract syntax
###########################################################################
class ClassLoader:
    def Run(self):
        global parent_class_
        casal2_src_folder = '../CASAL2/source/'
        # List of source code folders to parse for syntax code
        # Add additional top-level folders here as required
        parent_class_folders = ['AdditionalPriors', 'AgeingErrors', 'AgeLengths', 'Asserts', 'Catchabilities', 'Categories',
                                'DerivedQuantities', 'Estimates', 'EstimateTransformations', 'InitialisationPhases',
                                'LengthWeights', 'Likelihoods', 'MCMCs', 'Minimisers', 'Model', 'Observations', 'Penalties',
                                'Processes', 'Profiles', 'Projects', 'Reports', 'Selectivities', 'Simulates', 'TimeSteps',
                                'TimeVarying']
        type_to_exclude_third_level = ['Minimisers']
        for folder in parent_class_folders:
            print('\n--> Scanning for files in ' + folder + '')
            parent_class_ = Class()
            # Start with base class folder, then common/age/length folders
            if (os.path.exists(casal2_src_folder + folder + '/')):
                label_ = Variable()
                label_.name_ = 'label'
                label_.type_ = 'string'
                parent_class_.variables_['label_'] = label_
                type_ = Variable()
                type_.name_ = 'type'
                type_.type_ = 'string'
                parent_class_.variables_['type_'] = type_
                # This if statement deals with Classes that have parents in Common, it also
                # checks if it has children in Common, Age, or Length folders
                file_list = os.listdir(casal2_src_folder + folder + '/')
                for file in file_list:
                    if file.startswith(folder[:-3]) and file.endswith('.h') and not file.endswith('-inl.h'):
                        print('    Loading top-level parent class from file "' + file + '"')
                        parent_class_.name_ = file.replace('.h', '')
                        if not VariableLoader().Load('../CASAL2/source/' + folder + '/' + file, parent_class_):
                            return False
                        break

                print('    Scanning for files in ' + folder + '/Models/')
                if os.path.exists(casal2_src_folder + folder + '/Models/'):
                    child_file_list = os.listdir(casal2_src_folder + folder + '/Models/')
                    # Scan First For 2nd Level Common
                    for file in child_file_list:
                        if not file.endswith('.h'):
                            continue
                        if not file.startswith('Age'):
                            continue
                        child_class = Class()
                        child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                        child_class.variables_['label_'].name_ = ''
                        child_class.variables_['type_'].name_ = ''
                        child_class.name_ = file.replace('.h', '')
                        parent_class_.child_classes_[child_class.name_] = child_class
                        if not VariableLoader().Load('../CASAL2/source/' + folder + '/Models/' + file, child_class):
                            return False

                print('    Scanning for files in ' + folder + '/Common/')
                if os.path.exists(casal2_src_folder + folder + '/Common/'):
                    child_file_list = os.listdir(
                        casal2_src_folder + folder + '/Common/')
                    # Scan First For 2nd Level Common
                    for file in child_file_list:
                        if not file.endswith('.h'):
                            continue
                        child_class = Class()
                        child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                        child_class.variables_['label_'].name_ = ''
                        child_class.variables_['type_'].name_ = ''
                        child_class.name_ = file.replace('.h', '')
                        parent_class_.child_classes_[child_class.name_] = child_class
                        if not VariableLoader().Load('../CASAL2/source/' + folder + '/Common/' + file, child_class):
                            return False

                    # Scan 3rd Level Children in Common
                    print('    Scanning for third level children in ' + folder + '/Common/')
                    for file in child_file_list:
                        # print file
                        if os.path.isdir(casal2_src_folder + folder + '/Common/' + file):
                            if folder not in type_to_exclude_third_level:
                                child_file_list = os.listdir(casal2_src_folder + folder + '/Common/' + file)
                                for child_file in child_file_list:
                                    if not child_file.endswith('.h'):
                                        continue
                                    if file not in parent_class_.child_classes_:
                                        return Globals.PrintError('Child class ' + file + ' was not found in ' + parent_class_.name_ + ' Common classes')
                                    sub_child_class = copy.deepcopy(parent_class_.child_classes_[file])
                                    sub_child_class.name_ = child_file.replace('.h', '') + file
                                    sub_child_class.parent_name_ = file
                                    parent_class_.child_classes_[file].child_classes_[sub_child_class.name_] = sub_child_class
                                    if not VariableLoader().Load('../CASAL2/source/' + folder + '/Common/' + file + '/' + child_file, sub_child_class):
                                        return False

                # Go through Age folders
                print('    Scanning for files in ' + folder + '/Age/')
                if (os.path.exists(casal2_src_folder + folder + '/Age/')):
                    child_file_list = os.listdir(casal2_src_folder + folder + '/Age/')
                    # Scan First For 2nd Level Children
                    for file in child_file_list:
                        if not file.endswith('.h'):
                            continue
                        print('    Creating class for ' + folder + '/Age/' + file)
                        child_class = Class()
                        child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                        child_class.variables_['label_'].name_ = ''
                        child_class.variables_['type_'].name_ = ''
                        child_class.name_ = file.replace('.h', '')
                        parent_class_.child_classes_[child_class.name_] = child_class
                        if not VariableLoader().Load('../CASAL2/source/' + folder + '/Age/' + file, child_class):
                            return False

                # Go through Length folders
                # print('    Scanning for files in ' + folder + '/Length/')
                # if (os.path.exists(casal2_src_folder + folder + '/Length/')):
                #    common_file_list = os.listdir(casal2_src_folder + folder + '/Length/')
                #    # Scan First For 2nd Level Children
                #    for file in common_file_list:
                #        if not file.endswith('.h'):
                #            continue
                #        print('    Creating class for ' + folder + '/Length/' + file)
                #        common_class = Class()
                #        common_class.variables_ = copy.deepcopy(parent_class_.variables_)
                #        common_class.variables_['label_'].name_ = ''
                #        common_class.variables_['type_'].name_ = ''
                #        common_class.name_ = file.replace('.h', '')
                #        parent_class_.child_classes_[common_class.name_] = common_class
                #        if not VariableLoader().Load('../CASAL2/source/' + folder + '/Length/' + file, common_class):
                #            return False

            parent_class_.child_classes_ = collections.OrderedDict(sorted(parent_class_.child_classes_.items()))
            Printer().Run()
        return True


###########################################################################
# Read the header/cpp files and load variables
###########################################################################
class VariableLoader:
    def Load(self, header_file_, class_):
        self.LoadHeaderFile(header_file_, class_)
        return self.LoadCppFile(header_file_, class_)

    def LoadHeaderFile(self, header_file_, class_):
        print('    Loading Variables for ' + class_.name_ + ' from header file ' + header_file_)
        fi = fileinput.FileInput(header_file_)
        found_class = False
        for line in fi:
            line = line.rstrip().lstrip()
            if line == '':
                continue
            if not found_class and line.startswith('class') and line.endswith('{'):
                found_class = True
                continue
            if not found_class:
                continue
            if line.startswith('//') or line.startswith('/*') or line.startswith('*') or line.startswith('}') or line.startswith('#'):
                continue
            if line == 'public:' or line == 'protected:' or line == 'private:':
                continue
            if '(' in line or ')' in line or line.startswith('template'):
                continue

            # Shrink any gaps we have have in the declaration of the type, e.g vector< vector<string> >
            line = line.replace('< ', '<').replace('> >', '>>').replace(';', '').replace(', ', ',')
            pieces = line.split()
            if len(pieces) < 2:
                continue

            variable = Variable()
            variable.type_ = pieces[0]
            class_.variables_[pieces[1]] = variable
            print('    Heading Variable: ' + pieces[1] + '(' + pieces[0] + ')')

    def LoadCppFile(self, header_file_, class_):
        cpp_file = header_file_.replace('.h', '.cpp')
        constructor_line = class_.name_ + '::' + class_.name_ + '('
        print('    Loading Variables from CppFile ' + cpp_file)
        print('    Looking for constructor line: ' + constructor_line)
        fi = fileinput.FileInput(cpp_file)
        in_constructor = False
        finished_constructor_definition = False
        previous_line = ''
        for line in fi:
            line = line.rstrip().lstrip()
            if line == '' or line == '\n':
                continue
            if line.startswith(constructor_line):
                in_constructor = True
                if line.endswith('{'):
                    finished_constructor_definition = True
                continue
            if not in_constructor:
                continue
            if line.endswith('{') and not finished_constructor_definition:
                finished_constructor_definition = True
                continue
            if not finished_constructor_definition:
                continue
            if line == '}' and in_constructor:
                break
            if not line.endswith(';'):
                previous_line = previous_line + line
                continue
            if previous_line != '':
                line = previous_line + line
            previous_line = ''
            if line.startswith('parameters_.Bind<'):
                print('    Bind Line ' + line)
                if not self.HandleParameterBindLine(line, class_):
                    return False
            if line.startswith('parameters_.BindTable('):
                print('    Bind Table Line ' + line)
                if not self.HandleParameterBindTable(line, class_):
                    return False
            if line.startswith('RegisterAsAddressable('):
                print('    RegisterAsAddressable Line ' + line)
                if not self.HandleRegisterAsLine(line, class_):
                    return False
            # if line.startswith(LaTeX_line):
            #    print('    LaTeX Line ' + line)
            #    if not self.HandleLaTeXAsLine(line, class_):
            #        return False
        return True

    # def HandleLaTeXAsLine(self, line, class_):

    def HandleParameterBindLine(self, line, class_):
        line = line.replace('Double(', '').replace('double(', '')
        lines = re.split('->', line)
        short_line = lines[0].replace('parameters_.Bind<', '')
        # Break the line into groups, ignoring special characters inside quotes
        pieces = re.sub(reCapitalLetters, "\n", short_line)
        pieces = re.split('\n', pieces)
        pieces = list(filter(None, pieces))
        # Check for the name of the variable we're binding too
        used_variable = pieces[2].replace('&', '').rstrip().lstrip()
        print('      short_line ' + short_line)
        print('      used variable ' + used_variable)
        # Convert underscores to LaTeX underscores
        variable = Variable()
        if used_variable in class_.variables_:
            variable = class_.variables_[used_variable]
        else:
            return Globals.PrintError('Could not find record for the header variable: ' + used_variable)

        # Check for the Name
        variable.name_ = translations_[
            pieces[1].replace('(', '').rstrip().lstrip()]
        class_.variable_order_.append(used_variable)
        # Set the description
        index = 3
        description = pieces[index]
        while description.count('"') != 2:
            index += 1
            description += ', ' + pieces[index]
        variable.description_ = description.replace('"', '').replace('\\\\', '\\').replace('_', '\_').rstrip().lstrip()
        if (variable.description_.startswith('R(')):
            variable.description_ = variable.description_[2:len(variable.description_)]
        # Set the value
        index += 1
        value = pieces[index]
        while value.count('"') != 2:
            index += 1
            value += ' ' + pieces[index]
        value = value.replace(')', '')
        variable.value_ = value.replace(')', '').replace('"', '').replace('\\\\', '\\').replace('_', '\_').rstrip().lstrip()
        if (variable.value_.startswith('R(')):
            variable.value_ = variable.value_[2:len(variable.value_)]
        # Set the default value
        index += 1
        if len(pieces) > index:
            variable.default_ = pieces[index].replace(')', '').replace('_', '\_').replace('\\\\', '\\').rstrip().lstrip()
        if (variable.default_.startswith('R(')):
            variable.default_ = variable.default_[2:len(variable.default_)]

        if len(lines) == 2:
            short_line = lines[1]
            if short_line.startswith('set_lower_bound'):
                lower_bound_info = short_line.replace('set_lower_bound(', '').replace(');', '').replace('u', '').split(',')
                lower_bound = lower_bound_info[0]
                if len(lower_bound_info) == 2 and lower_bound_info[1].lstrip().rstrip().lower() == 'false':
                    lower_bound += ' (exclusive)'
                else:
                    lower_bound += ' (inclusive)'
                variable.lower_bound_ = lower_bound
            elif short_line.startswith('set_upper_bound'):
                upper_bound_info = short_line.replace('set_upper_bound(', '').replace(');', '').replace('u', '').split(',')
                upper_bound = upper_bound_info[0]
                if len(upper_bound_info) == 2 and upper_bound_info[1].lstrip().rstrip().lower() == 'false':
                    upper_bound += ' (exclusive)'
                else:
                    upper_bound += ' (inclusive)'
                variable.upper_bound_ = upper_bound
            elif short_line.startswith('set_range('):
                info = short_line.replace('set_range(', '').replace(');', '').split(',')
                lower_bound = info[0].rstrip().lstrip()
                upper_bound = info[1].rstrip().lstrip()
                if len(info) > 2 and info[2].lstrip().rstrip().lower() == 'false':
                    lower_bound += ' (exclusive)'
                else:
                    lower_bound += ' (inclusive)'
                if len(info) > 3 and info[3].lstrip().rstrip().lower() == 'false':
                    upper_bound += ' (exclusive)'
                else:
                    upper_bound += ' (inclusive)'
                variable.lower_bound_ = lower_bound
                variable.upper_bound_ = upper_bound
            elif short_line.startswith('set_allowed_values('):
                info = short_line.replace('set_allowed_values({', '').replace('});', '').replace('"', '').split(',')
                allowed_values = []
                for value in info:
                    value = value.strip().lstrip()
                    if value.startswith('PARAM'):
                        allowed_values.append(translations_[value])
                    else:
                        allowed_values.append(value)
                short_line = ', '.join(allowed_values)
                variable.allowed_values_ = short_line
        return True

    def HandleParameterBindTable(self, line, class_):
        lines = re.split('->', line)
        short_line = line.replace('parameters_.BindTable(', '')
        pieces = re.split(',|<|>|;|\(|\)', short_line)
        pieces = list(filter(None, pieces))

        # Check for the name of the variable we're binding too
        used_variable = pieces[1].rstrip().lstrip()

        variable = Variable()
        if used_variable in class_.variables_:
            variable = class_.variables_[used_variable]
        else:
            return Globals.PrintError('Could not find record for the header variable: ' + used_variable)

        # Check for Name
        variable.name_ = translations_[pieces[0].rstrip().lstrip()]

        # Set the description
        index = 2
        description = pieces[index]
        while description.count('"') != 2:
            index += 1
            description += ',' + pieces[index]
        variable.description_ = description.replace(')', '').replace('"', '').rstrip().lstrip()

        # Set the value
        index += 1
        value = pieces[index]
        while value.count('"') != 2:
            index += 1
            value += ',' + pieces[index]
        value = value.replace(')', '')
        variable.value_ = value.replace(')', '').replace('"', '').rstrip().lstrip()

        # Set the default value
        index += 1
        if len(pieces) > index:
            variable.default_ = pieces[index].replace(')', '').rstrip().lstrip()

        return True

    def HandleRegisterAsLine(self, line, class_):
        short_line = line.replace('RegisterAsAddressable(', '')
        pieces = re.split(',|<|>|;|\(|\)', short_line)
        pieces = list(filter(None, pieces))

        if len(pieces) != 3 and len(pieces) != 2 and len(pieces) != 1:
            return Globals.PrintError('Expected 2 or 3 pieces but got ' + str(len(pieces)) + ' with line: ' + line)

        if len(pieces) == 1:
            name = 'user defined'
            variable = pieces[0].replace('&', '').replace(')', '').lstrip().rstrip()
            lookup = "all"
        elif len(pieces) == 2:
            name = pieces[0]
            variable = pieces[1].replace('&', '').replace(')', '').lstrip().rstrip()
            lookup = "all"
        else:
            name = pieces[0]
            variable = pieces[1].replace('&', '').replace(')', '').lstrip().rstrip()
            lookup = pieces[2].replace('addressable::k', '').replace(')', '').lstrip().rstrip()

        # At some point it would be nice to add the lookup into the auto-documentation but that can wait.
        print('--> Estimable: ' + name + ' with variable ' + variable + ' lookup = ' + lookup + ' ' + class_.variables_[variable].name_)
        if name in translations_:
            name = translations_[name]
        print(class_.variables_)
        if lookup == "all":
            class_.estimables_[name] = class_.variables_[variable].type_
            print('--> Estimable: ' + name + ' as type ' + class_.estimables_[name])
        else:
            class_.addressables_[name] = class_.variables_[variable].type_
            # This will not deal with the following case. When a object is user defined in the constructor, and has an addressable not 'all'
            print('--> Addressable: ' + name + ' as type ' + class_.addressables_[name] + ' ' + class_.variables_[variable].name_)
            variable_temp = Variable()
            variable_temp.name_ = name
            class_.variables_[name] = variable_temp
            class_.variable_order_.append(name)
        return True


class Printer:
    output_path_ = '../Documentation/UserManual/Syntax/'

    def Run(self):
        global parent_class_
        print('--> Running Latex Printer')
        print('--> Top Class ' + parent_class_.name_)
        self.current_output_file_ = self.output_path_ + parent_class_.name_

        parent_class_.name_ = re.sub(reReplaceUnderscores, '\_', parent_class_.name_)

        if not os.path.exists('../Documentation/UserManual/Syntax/'):
            os.makedirs('../Documentation/UserManual/Syntax/')

        print('-- Printing to file ' + self.current_output_file_)
        file = open(self.current_output_file_ + '.tex', 'w')
        file.write('\defComLab{' + parent_class_.name_ + '}{Define an object of type \emph{' + parent_class_.name_ + '}}.\n')
        file.write('\\defRef{sec:' + parent_class_.name_.replace('\_', '') + '}\n')
        file.write('\\label{syntax:' + parent_class_.name_.replace('\_', '') + '}\n\n')
        self.PrintClass(file, parent_class_)

        parent_class_.child_classes_ = collections.OrderedDict(
            sorted(parent_class_.child_classes_.items()))
        for child_class_name, child_class in parent_class_.child_classes_.items():
            object_name = re.sub(reReplaceUnderscores, '\_', child_class.name_.replace('_', '\_').replace('\\\\', '\\'))
            class_name = re.sub(reReplaceUnderscores, '\_', child_class.name_.replace('_', '\_').replace('\\\\', '\\'))
            parent_class = re.sub(reReplaceUnderscores, '\_', parent_class_.name_.replace('_', '\_').replace('\\\\', '\\'))
            # Exception corrections
            class_name = class_name.replace('GammaDiff', 'Numerical\_Differences')
            # write file
            file.write('\subsubsection{' + parent_class + ' of type ' + object_name + '}\n')
            file.write('\\commandlabsubarg{' + parent_class + '}{type}{' + class_name + '}.\n')
            file.write('\\defRef{sec:' + parent_class.replace('\_', '') + '-' + class_name.replace('\_', '') + '}\n')
            file.write('\\label{syntax:' + parent_class.replace('\_', '') + '-' + class_name.replace('\_', '') + '}\n\n')
            CommandCount = self.PrintClass(file, child_class)
            if(CommandCount == 0):
                file.write('The ' + class_name + ' type has no additional subcommands.\n')
        file.close()
        return True

    def PrintClass(self, file_, class_):
        CommandCount = 0
        class_.estimables_ = collections.OrderedDict(sorted(class_.estimables_.items()))
        for key in class_.variable_order_:
            CommandCount = CommandCount + 1
            print(key)
            variable = class_.variables_[(key)]
            if variable.name_ == '':
                continue
            print(variable.name_ + ' ' + class_.name_)
            # Remove PARAMs and associated descriptions of variables that have yet to be completed in the code
            if variable.description_.startswith('TBA'):
                continue
            # And continue as normal
            file_.write('\\defSub{' + variable.name_ + '}{' + variable.description_.replace('\\\\', '\\') + '}\n')
            if variable.name_ in class_.estimables_:
                if class_.estimables_[variable.name_].startswith('vector<') or class_.estimables_[variable.name_].startswith('map<'):
                    file_.write('\\defType{Vector of real numbers (estimable)}\n')
                else:
                    file_.write('\\defType{Real number (estimable)}\n')
            elif variable.name_ in class_.addressables_:
                if class_.addressables_[variable.name_].startswith('vector<') or class_.addressables_[variable.name_].startswith('map<'):
                    file_.write('\\defType{Vector of addressables}\n')
                else:
                    file_.write('\\defType{Addressable}\n')
            else:
                file_.write('\\defType{' + type_aliases_[variable.type_] + '}\n')
            if variable.default_.replace('\_', '_').startswith('PARAM_'):
                file_.write('\\defDefault{' + translations_[variable.default_.replace('\_', '_')] + '}\n')
            else:
                variable.default_ = re.sub(reRemoveU, '', variable.default_)
                file_.write('\\defDefault{' + variable.default_.replace('""', 'No default') + '}\n')
            if variable.value_ != '':
                if variable.allowed_values_ != '':
                    file_.write('\\defValue{' + variable.value_ + ' (' + variable.allowed_values_ + ')}\n')
                else:
                    file_.write('\\defValue{' + variable.value_ + '}\n')
            if variable.lower_bound_ != '':
                variable.lower_bound_ = re.sub(reRemoveU, '', variable.lower_bound_)
                file_.write('\\defLowerBound{' + variable.lower_bound_ + '}\n')
            if variable.upper_bound_ != '':
                variable.upper_bound_ = re.sub(reRemoveU, '', variable.upper_bound_)
                file_.write('\\defUpperBound{' + variable.upper_bound_ + '}\n')
            if variable.note_ != '':
                file_.write('\\defNote{' + variable.note_ + '}\n')

            file_.write('\n')
        return CommandCount


class Latex:
    def Build(self):
        print('-- Building User Manual documentation')
        cwd = os.path.normpath(os.getcwd())
        os.chdir('../Documentation/UserManual/')
        print('-- Building CASAL.syn')
        os.system('python QuickReference.py')

        for i in range(0, 3):
            if Globals.operating_system_ != "windows":
                # Create CASAL2.aux
                if os.system('pdflatex --interaction=nonstopmode CASAL2') != EX_OK:
                    return False
                # Create the bibliography
                if os.system('bibtex CASAL2') != EX_OK:
                    return False
                # Fix the references/citations
                if os.system('pdflatex --halt-on-error --interaction=nonstopmode CASAL2') != EX_OK:
                    return False
                if os.system('makeindex CASAL2') != EX_OK:
                    return False
                if not os.path.exists('CASAL2.pdf'):
                    return False
            else:
                if os.system('pdflatex.exe --enable-installer CASAL2') != EX_OK:
                    return Globals.PrintError('pdflatex failed')
                if os.system('bibtex.exe CASAL2') != EX_OK:
                    return Globals.PrintError('bibtex failed')
                if os.system('pdflatex.exe --halt-on-error --enable-installer CASAL2') != EX_OK:
                    return Globals.PrintError('pdflatex failed')
                if os.system('makeindex.exe CASAL2') != EX_OK:
                    return Globals.PrintError('makeindex failed')
        print('-- Built the Casal2 User Manual')

        return True
