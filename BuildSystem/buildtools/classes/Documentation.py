import os
import sys
import subprocess
import os.path
import fileinput
import re
import collections
import copy
from datetime import datetime, date
from dateutil import tz

import Globals

EX_OK = getattr(os, "EX_OK", 0)

# Object Storing Class
class Variable:
    def __init__(self):
        self.allowed_values_ = ""
        self.name_ = ""
        self.type_ = ""
        self.description_ = ""
        self.value_ = ""
        self.default_ = "No Default"
        self.lower_bound_ = ""
        self.upper_bound_ = ""

    def Print(self):
        print 'Variable: ' + self.name_ + '; type: ' + self.type_
        #+ '; description: ' + self.description_
        #+ '; value ' + self.value_
        #+ '; default: ' + self.default_
        #+ '; lower_bound: ' + self.lower_bound_
        #+ '; upper_bound: ' + self.upper_bound_
        #+ '; allowed_values: ' + ', '.join(self.allowed_values_)

class Class:
    def __init__(self):
        self.name_ = "No Name"
        self.parent_name_ = ""
        self.variables_ = {}
        self.estimables_ = {}
        self.addressables_ = {}
        self.child_classes_  = {}
        self.variable_order_ = []

# Variables we want to use
type_aliases_ = {} # Type Aliases convert C++ types to English text
translations_ = {} # Translations stores all of our PARAM_X and the English text
parent_class_ = Class() # Hold our top most parent class (e.g niwa::Process)

class Documentation:
    # Methods
    def __init__(self):
        print '--> Starting Documentation Builder'
        type_aliases_['double']           = 'constant'
        type_aliases_['unsigned']         = 'non-negative integer'
        type_aliases_['bool']             = 'boolean'
        type_aliases_['int']              = 'integer'
        type_aliases_['string']           = 'string'
        type_aliases_['vector<double>']   = 'constant vector'
        type_aliases_['vector<unsigned>'] = 'non-negative integer vector'
        type_aliases_['vector<bool>']     = 'boolean vector'
        type_aliases_['vector<int>']      = 'integer vector'
        type_aliases_['vector<string>']   = 'string vector'
        type_aliases_['map<string, vector<string>>'] = 'string matrix'
        type_aliases_['map<string, vector<Double>>'] = 'constant vector'
        type_aliases_['parameters::table*']          = 'constant'
        
    """
    Start the documentation builder
    """
    def start(self):
        # Figure out if we have Latex in our path, if we do then we want to
        # build the PDF documentation as well
        if Globals.latex_path_ != "":
            self.build_latex_ = True
        # Load our translations from the Translations.h file so we can convert
        # PARAM_X in to actual English
        if not self.load_translations():
            return False

        if not ClassLoader().Run():
            return False

        return Latex().Build()

    """
    Load the translations from our translation file. This informaiton gets stored in a map
    so it can be used for converting the PARAM_X names in to English when we print the details
    to the file
    """
    def load_translations(self):
        print '--> Loading translations'
        file = fileinput.FileInput('../CASAL2/source/Common/Translations/English_UK.h')
        if not file:
            return Globals.PrintError('Failed to open the English_UK.h for translation loading')

        for line in file:
            if not line.startswith('#define'):
                continue
            pieces = line.split()
            if len(pieces) < 3 or not pieces[1].startswith('PARAM'):
                continue

            lookup = pieces[1];
            value = pieces[2].lstrip().rstrip().replace('"', '').replace('_', '\_')
            translations_[lookup] = value

        print '-- Loaded ' + str(len(translations_)) + ' translation values'
        return True


class ClassLoader:
    def Run(self):
        global parent_class_
        casal2_src_folder = '../CASAL2/source/Common/'
        casal2_age_src_folder = '../CASAL2/source/Age/'
        casal2_length_src_folder = '../CASAL2/source/Length/'
		
        parent_class_folders = [ 'AdditionalPriors', 'AgeingErrors', 'AgeLengths', 'Asserts',
                        'Catchabilities', 'Categories', 'DerivedQuantities',
                        'Estimates', 'EstimateTransformations', 'InitialisationPhases', 'LengthWeights',
                        'Likelihoods', 'MCMCs', 'Minimisers',
                        'Model', 'Observations', 'Penalties', 'Processes', 'Profiles', 'Projects',
                        'Reports', 'Selectivities', 'Simulates', 'TimeSteps', 'TimeVarying']
        type_without_children_folders = [ 'Model', 'Profiles', 'TimeSteps' ]
        type_to_exclude_third_level_children = [ 'Minimisers' ]
        for folder in parent_class_folders:
            parent_class_ = Class()
            print folder
			# Start with Common folder, but know we also go through age and length folders
            if (os.path.exists(casal2_src_folder + folder) or folder in type_without_children_folders):
                label_ = Variable()
                label_.name_ = 'label'
                label_.type_ = 'string'
                parent_class_.variables_['label_'] = label_
                type_ = Variable()
                type_.name_ = 'type'
                type_.type_ = 'string'                
                parent_class_.variables_['type_'] = type_
                print type
                if (os.path.exists(casal2_src_folder + folder + '/')): 
                #This if statement deals with Classes that have parents in Common, it also checks if it has children in Age or Length
                    file_list = os.listdir(casal2_src_folder + folder + '/')
                    for file in file_list:
                        if file.startswith(folder[:-3]) and file.endswith('.h') and not file.endswith('-inl.h'):
                            print '-- Loading Common -> top-level parent class from file ' + file
                            parent_class_.name_ = file.replace('.h', '')
                            if not VariableLoader().Load('../CASAL2/source/Common/' + folder + '/' + file, parent_class_):
                                return False
                            break;

                    if os.path.exists(casal2_src_folder + folder + '/Children/'):
                        print '--> Scanning for children'
                        child_file_list = os.listdir(casal2_src_folder + folder + '/Children/')
                        # Scan First For 2nd Level Children
                        for file in child_file_list:
                            if not file.endswith('.h'):
                                continue
                            child_class = Class()
                            child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                            child_class.variables_['label_'].name_ = ''
                            child_class.variables_['type_'].name_ = ''
                            child_class.name_ = file.replace('.h', '')
                            parent_class_.child_classes_[child_class.name_] = child_class
                            if not VariableLoader().Load('../CASAL2/source/Common/' + folder + '/Children/' + file, child_class):
                                return False

                        # Scan 3rd Level Children
                        print '--> Scanning for Third Level Children'
                        for file in child_file_list:
                            #print file
                            if os.path.isdir(casal2_src_folder + folder + '/Children/' + file):
                                if folder not in type_to_exclude_third_level_children:
                                    child_file_list = os.listdir(casal2_src_folder + folder + '/Children/' + file)
                                    for child_file in child_file_list:
                                        if not child_file.endswith('.h'):
                                            continue
                                        if file not in parent_class_.child_classes_:
                                            return Globals.PrintError('Child class ' + file + ' was not found in ' + parent_class_.name_ + ' child classes')

                                        sub_child_class = copy.deepcopy(parent_class_.child_classes_[file])
                                        sub_child_class.name_ = child_file.replace('.h', '') + file
                                        sub_child_class.parent_name_ = file
                                        print 'child file ' + sub_child_class.name_
                                        
                                        parent_class_.child_classes_[file].child_classes_[sub_child_class.name_] = sub_child_class
                                        if not VariableLoader().Load('../CASAL2/source/Common/' + folder + '/Children/' + file + '/' + child_file, sub_child_class):
                                            return False

                        # Go through Age folders
                        print '--- Does Source/Age have any children folder named ' + folder
                if (os.path.exists(casal2_age_src_folder + folder + '/Children/')):
                    print '--> Scanning for file in source/Age/' + folder
                    child_file_list = os.listdir(casal2_age_src_folder + folder + '/Children/')
                    # Scan First For 2nd Level Children
                    for file in child_file_list:
                        if not file.endswith('.h'):
                            continue
                        print '--> Creating class for Age/Source/' + folder + '/' + file
                        child_class = Class()
                        child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                        child_class.variables_['label_'].name_ = ''
                        child_class.variables_['type_'].name_ = ''
                        child_class.name_ = file.replace('.h', '')
                        parent_class_.child_classes_[child_class.name_] = child_class
                        if not VariableLoader().Load('../CASAL2/source/Age/' + folder + '/Children/' + file, child_class):
                            return False

                if (os.path.exists(casal2_length_src_folder + folder + '/Children/')):
                    print '--> Scanning for following child in Length: ' + folder
                    child_file_list = os.listdir(casal2_length_src_folder + folder + '/Children/')
                    # Scan First For 2nd Level Children
                    for file in child_file_list:
                        if not file.endswith('.h'):
                            continue
                        child_class = Class()
                        child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                        child_class.variables_['label_'].name_ = ''
                        child_class.variables_['type_'].name_ = ''
                        child_class.name_ = file.replace('.h', '')
                        parent_class_.child_classes_[child_class.name_] = child_class
                        if not VariableLoader().Load('../CASAL2/source/Length/' + folder + '/Children/' + file, child_class):
                            return False
                
            elif (os.path.exists(casal2_age_src_folder + folder + '/')):
                print '---Parent class ' + folder + ' only in Age'
                #This if statement deals with Classes that have parents only in Age
                label_ = Variable()
                label_.name_ = 'label'
                label_.type_ = 'string'
                parent_class_.variables_['label_'] = label_
                type_ = Variable()
                type_.name_ = 'type'
                type_.type_ = 'string'                
                parent_class_.variables_['type_'] = type_
                if (os.path.exists(casal2_age_src_folder + folder + '/')): 
                    file_list = os.listdir(casal2_age_src_folder + folder + '/')
                    for file in file_list:
                        if file.startswith(folder[:-3]) and file.endswith('.h') and not file.endswith('-inl.h'):
                            print '-- Loading Age -> top-level parent class from file ' + file
                            parent_class_.name_ = file.replace('.h', '')
                            if not VariableLoader().Load('../CASAL2/source/Age/' + folder + '/' + file, parent_class_):
                                return False
                            break;

                    if os.path.exists(casal2_age_src_folder + folder + '/Children/'):
                        print '--> Scanning for children'
                        child_file_list = os.listdir(casal2_age_src_folder + folder + '/Children/')
                        # Scan First For 2nd Level Children
                        for file in child_file_list:
                            if not file.endswith('.h'):
                                continue
                            child_class = Class()
                            child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                            child_class.variables_['label_'].name_ = ''
                            child_class.variables_['type_'].name_ = ''
                            child_class.name_ = file.replace('.h', '')
                            parent_class_.child_classes_[child_class.name_] = child_class
                            if not VariableLoader().Load('../CASAL2/source/Age/' + folder + '/Children/' + file, child_class):
                                return False

            elif (os.path.exists(casal2_length_src_folder + folder + '/')):
                print '---Parent class ' + folder + ' only in Length'
                #This if statement deals with Classes that have parents only in Length
                label_ = Variable()
                label_.name_ = 'label'
                label_.type_ = 'string'
                parent_class_.variables_['label_'] = label_
                type_ = Variable()
                type_.name_ = 'type'
                type_.type_ = 'string'                
                parent_class_.variables_['type_'] = type_
                if (os.path.exists(casal2_length_src_folder + folder + '/')): 
                    file_list = os.listdir(casal2_length_src_folder + folder + '/')
                    for file in file_list:
                        if file.startswith(folder[:-3]) and file.endswith('.h') and not file.endswith('-inl.h'):
                            print '-- Loading Length -> top-level parent class from file ' + file
                            parent_class_.name_ = file.replace('.h', '')
                            if not VariableLoader().Load('../CASAL2/source/Length/' + folder + '/' + file, parent_class_):
                                return False
                            break;

                    if os.path.exists(casal2_length_src_folder + folder + '/Children/'):
                        print '--> Scanning for children'
                        child_file_list = os.listdir(casal2_length_src_folder + folder + '/Children/')
                        # Scan First For 2nd Level Children
                        for file in child_file_list:
                            if not file.endswith('.h'):
                                continue
                            child_class = Class()
                            child_class.variables_ = copy.deepcopy(parent_class_.variables_)
                            child_class.variables_['label_'].name_ = ''
                            child_class.variables_['type_'].name_ = ''
                            child_class.name_ = file.replace('.h', '')
                            parent_class_.child_classes_[child_class.name_] = child_class
                            if not VariableLoader().Load('../CASAL2/source/Length/' + folder + '/Children/' + file, child_class):
                                return False
            
            parent_class_.child_classes_ = collections.OrderedDict(sorted(parent_class_.child_classes_.items()))
            Printer().Run()
        return True

class VariableLoader:
    def Load(self, header_file_, class_):
        self.LoadHeaderFile(header_file_, class_)
        return self.LoadCppFile(header_file_, class_)

    def LoadHeaderFile(self, header_file_, class_):
        print '--> Loading Variables for ' + class_.name_ + ' from header file ' + header_file_
        fi = fileinput.FileInput(header_file_)
        found_class = False
        for line in fi:
            line = line.lower().rstrip().lstrip()
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

            # Shrink any gaps we have have in the declarion of the type
            # e.g vector< vector<string> >
            line = line.replace('< ', '<').replace('> >', '>>').replace(', ', ',').replace(';', '')
            pieces = line.split();
            if len(pieces) < 2:
                continue

            variable = Variable()
            variable.type_ = pieces[0]
            class_.variables_[pieces[1]] = variable            
            print '-- Heading Variable: ' + pieces[1] + '(' + pieces[0] + ')'

    def LoadCppFile(self, header_file_, class_):
        cpp_file = header_file_.replace('.h', '.cpp')
        constructor_line = class_.name_ + '::' + class_.name_ + '('

        print '--> Loading Variables from CppFile ' + cpp_file
        print '-- Looking for constructor line: ' + constructor_line
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
                print '-- Bind Line ' + line
                if not self.HandleParameterBindLine(line, class_):
                    return False
            if line.startswith('parameters_.BindTable('):
                print '-- Bind Table Line ' + line
                if not self.HandleParameterBindTable(line, class_):
                    return False            
            if line.startswith('RegisterAsAddressable('):
                print '-- RegisterAsAddressable Line ' + line
                if not self.HandlRegisterAsLine(line, class_):
                    return False
        return True

    def HandleParameterBindLine(self, line, class_):
        line = line.replace('Double(', '') 
        lines = re.split('->', line)
        short_line = lines[0].replace('parameters_.Bind<', '')
        pieces = re.split(',|<|>|;|(|)', short_line)
        pieces = filter(None, pieces)

        # Check for the name of the variable we're binding too
        used_variable = pieces[2].replace('&', '').rstrip().lstrip().lower()

        variable = Variable()
        if used_variable in class_.variables_:
            variable = class_.variables_[used_variable];
        else:
            return Globals.PrintError('Could not find record for the header variable: ' + used_variable)

        # Check for Name
        variable.name_ = translations_[pieces[1].replace('(', '').rstrip().lstrip()]
        class_.variable_order_.append(used_variable)
        # Set the description
        index = 3;
        description = pieces[index]
        while description.count('"') != 2:
            index += 1
            description += ',' + pieces[index]
        variable.description_ = description.replace('R"(', '').replace(')"', '').replace('"', '').rstrip().lstrip()

        # Set the value
        index += 1
        value = pieces[index]
        while value.count('"') != 2:
            index += 1
            value += ',' + pieces[index]
        value = value.replace(')', '')
        variable.value_ = value.replace(')', '').replace('R"(', '').replace(')"', '').replace('"', '').replace(')', '').rstrip().lstrip()

        # Set the default value
        index += 1
        if len(pieces) > index:
            variable.default_ = pieces[index].replace(')', '').rstrip().lstrip()

        if len(lines) == 2:
            short_line = lines[1]
            if short_line.startswith('set_lower_bound'):
                lower_bound_info = short_line.replace('set_lower_bound(', '').replace(');', '').split(',')
                lower_bound = lower_bound_info[0]
                if len(lower_bound_info) == 2 and lower_bound_info[1].lstrip().rstrip().lower() == 'false':
                    lower_bound += ' (exclusive)'
                else:
                    lower_bound += ' (inclusive)'
                variable.lower_bound_ = lower_bound
            elif short_line.startswith('set_upper_bound'):
                upper_bound_info = short_line.replace('set_upper_bound(', '').replace(');', '').split(',')
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
        lines  = re.split('->', line)    
        short_line = line.replace('parameters_.BindTable(', '')
        pieces = re.split(',|<|>|;|(|)', short_line)
        pieces = filter(None, pieces)

        # Check for the name of the variable we're binding too
        used_variable = pieces[1].rstrip().lstrip().lower()

        variable = Variable()
        if used_variable in class_.variables_:
            variable = class_.variables_[used_variable];
        else:
            return Globals.PrintError('Could not find record for the header variable: ' + used_variable)
            
        # Check for Name
        variable.name_ = translations_[pieces[0].rstrip().lstrip()]

        # Set the description
        index = 2;
        description = pieces[index]
        while description.count('"') != 2:
            index += 1
            description += ',' + pieces[index]
        variable.description_ = description.replace('R"(', '').replace(')"', '').replace('"', '').rstrip().lstrip()

        # Set the value
        index += 1
        value = pieces[index]
        while value.count('"') != 2:
            index += 1
            value += ',' + pieces[index]
        value = value.replace(')', '')
        variable.value_ = value.replace(')', '').replace('R"(', '').replace(')"', '').replace('"', '').replace(')', '').rstrip().lstrip()
 
        return True
 
         # Set the default value
        index += 1
        if len(pieces) > index:
            variable.default_ = pieces[index].replace(')', '').rstrip().lstrip()
        
    def HandlRegisterAsLine(self, line, class_):
        short_line = line.replace('RegisterAsAddressable(', '')
        pieces = re.split(',|<|>|;|(|)', short_line)
        pieces = filter(None, pieces)

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
	
	## At some point it would be nice to add the lookup into the auto-documentation but that can wait.
        print '--> Estimable: ' + name + ' with variable ' + variable + ' lookup = ' + lookup  + ' ' + class_.variables_[variable].name_
        if name in translations_:    
          name = translations_[name]
        print class_.variables_
  	if lookup == "all":
          class_.estimables_[name] = class_.variables_[variable].type_
          print '--> Estimable: ' + name + ' as type ' + class_.estimables_[name]
        else:
          class_.addressables_[name] = class_.variables_[variable].type_
          ## This will not deal with the following case. When a object is user defined in the constructor, and has an addressable not 'all'
          print '--> Addressable: ' + name + ' as type ' + class_.addressables_[name] + ' ' + class_.variables_[variable].name_
          variable_temp = Variable()
          variable_temp.name_ = name
          class_.variables_[name] = variable_temp
          class_.variable_order_.append(name)
        return True

class Printer:
    output_path_ = '../Documentation/UserManual/Syntax/'
    def Run(self):
        global parent_class_
        print '--> Running Latex Printer'
        print '--> Top Class ' + parent_class_.name_
        self.current_output_file_ = self.output_path_ + parent_class_.name_

        # Handle an Issue with MCMC
        if parent_class_.name_  != 'MCMC':
            parent_class_.name_  = re.sub( '(?<!^)(?=[A-Z])', '\_', parent_class_.name_)

        if not os.path.exists('../Documentation/UserManual/Syntax/'):
            os.makedirs('../Documentation/UserManual/Syntax/')

        print '-- Printing to file ' + self.current_output_file_
        file = open(self.current_output_file_ + '.tex', 'w')
        file.write('\defComLab{' + parent_class_.name_.lower() + '}{Define an object of type \emph{' + parent_class_.name_.lower() + '}}\n')
        file.write('\n')
        self.PrintClass(file, parent_class_)

        parent_class_.child_classes_ = collections.OrderedDict(sorted(parent_class_.child_classes_.items()))
        for child_class_name, child_class in parent_class_.child_classes_.iteritems():
            if len(child_class.child_classes_) > 0:
                child_class.child_classes_ = collections.OrderedDict(sorted(child_class.child_classes_.items()))
                for third_class_name, third_class in child_class.child_classes_.iteritems():
                    
                    ## this is an exception to remove time_step in front of mortality block observations
                    first_val = third_class.name_[0:8]                  
                    if parent_class_.name_ == 'Observation' and first_val == 'TimeStep':
                    	third_class.name_ = third_class.name_[8:len(third_class.name_)]
                    
                    object_name = re.sub( '(?<!^)(?=[A-Z])', ' ', third_class.name_)
                    class_name = re.sub( '(?<!^)(?=[A-Z])', '\_', third_class.name_).lower()
                    parent_class = re.sub( '(?<!^)(?=[A-Z])', '\_', parent_class_.name_).lower()
					   
                    # Exeption corrections
                    class_name = class_name.replace('m\_c\_m\_c', 'mcmc')
                    class_name = class_name.replace('beta\_diff', 'betadiff')
                    class_name = class_name.replace('gamma\_diff', 'numerical\_differences')    
                    class_name = class_name.replace('a\_d\_o\_l\_c', 'adolc')
                    class_name = class_name.replace('c\_p\_p\_a\_d', 'cppad')
                    class_name = class_name.replace('d\_e\_solver', 'de\_solver')
                    parent_class = parent_class.replace('m\_c\_m\_c', 'mcmc')
                    parent_class = parent_class.replace('a\_d\_o\_l\_c', 'adolc')
                    parent_class = parent_class.replace('c\_p\_p\_a\_d', 'cppad')
                    parent_class = parent_class.replace('d\_e\_solver', 'de\_solver')
                    object_name = object_name.replace('A D O L C', 'ADOLC')
                    object_name = object_name.replace('Beta Diff', 'Betadiff')
                    object_name = object_name.replace('C P P A D', 'CppAD')
                    object_name = object_name.replace('D E Solver', 'DE Solver')
                    object_name = object_name.replace('D Lib', 'DLib')
                    object_name = object_name.replace('M C M C', 'MCMC')
                    object_name = object_name.replace('M P D', 'MPD')
                    object_name = object_name.replace('With Q', 'With q')
                    # write file
                    ## Exception to remove the 'nop' process from the manual
                    if not (parent_class == 'process' and class_name == 'nop'):
                      file.write('\subsubsection[' + object_name + ']{\\commandlabsubarg{' + parent_class + '}{type}{' + class_name + '}}\n\n')
                      self.PrintClass(file, third_class)
            else:
                object_name = re.sub( '(?<!^)(?=[A-Z])', ' ', child_class.name_)
                class_name = re.sub( '(?<!^)(?=[A-Z])', '\_', child_class.name_).lower()
                parent_class = re.sub( '(?<!^)(?=[A-Z])', '\_', parent_class_.name_).lower()

 			   # Exeption corrections
                class_name = class_name.replace('m\_c\_m\_c', 'mcmc')
                class_name = class_name.replace('a\_d\_o\_l\_c', 'adolc')
                class_name = class_name.replace('beta\_diff', 'betadiff')                       
                class_name = class_name.replace('c\_p\_p\_a\_d', 'cppad')
                class_name = class_name.replace('gamma\_diff', 'numerical\_differences')     
                class_name = class_name.replace('d\_e\_solver', 'de\_solver')
                parent_class = parent_class.replace('m\_c\_m\_c', 'mcmc')
                parent_class = parent_class.replace('a\_d\_o\_l\_c', 'adolc')
                parent_class = parent_class.replace('c\_p\_p\_a\_d', 'cppad')
                parent_class = parent_class.replace('d\_e\_solver', 'de\_solver')
                object_name = object_name.replace('A D O L C', 'ADOLC')
                object_name = object_name.replace('Beta Diff', 'Betadiff')
                object_name = object_name.replace('C P P A D', 'CppAD')
                object_name = object_name.replace('D E Solver', 'DE Solver')
                object_name = object_name.replace('D Lib', 'DLib')
                object_name = object_name.replace('M C M C', 'MCMC')
                object_name = object_name.replace('M P D', 'MPD')
                object_name = object_name.replace('With Q', 'With q')
                # write file
                ## Exception to remove the 'nop' process from the manual
                if not (parent_class == 'process' and class_name == 'nop'):
                  file.write('\subsubsection[' + object_name + ']{\\commandlabsubarg{' + parent_class + '}{type}{' + class_name + '}}\n\n')
                  self.PrintClass(file, child_class)
        file.close()
        return True

    def PrintClass(self, file_, class_):
        class_.estimables_ = collections.OrderedDict(sorted(class_.estimables_.items()))
        for key in class_.variable_order_:
            print key
            variable = class_.variables_[(key)]
            if variable.name_ == '':
                continue
            print variable.name_ + ' ' + class_.name_
            # Remove PARAMs and associated desriptions of variables that have yet to be completed in the code
            if variable.description_.startswith('TBA'):
                continue
            # And continue as normal
            file_.write('\\defSub{' + variable.name_ + '} {' + variable.description_ + '}\n')            
            if variable.name_ in class_.estimables_:
                if class_.estimables_[variable.name_ ].startswith('vector<') or class_.estimables_[variable.name_ ].startswith('map<'):
                    file_.write('\\defType{estimable vector}\n')
                else:
                    file_.write('\\defType{estimable}\n')
            elif variable.name_ in class_.addressables_:
                if class_.addressables_[variable.name_ ].startswith('vector<') or class_.addressables_[variable.name_ ].startswith('map<'):
                    file_.write('\\defType{Addressable vector}\n')
                else:
                    file_.write('\\defType{Addressable}\n')                    
            else:
                file_.write('\\defType{' + type_aliases_[variable.type_] + '}\n')

            if variable.default_.startswith('PARAM_'):
                file_.write('\\defDefault{' + translations_[variable.default_] + '}\n')
            else:
                file_.write('\\defDefault{' + variable.default_ + '}\n')

            if variable.value_ != '':
                file_.write('\\defValue{' + variable.value_.replace('_', '\_') + '}\n')
            if variable.lower_bound_ != '':
                file_.write('\\defLowerBound{' + variable.lower_bound_ + '}\n')
            if variable.upper_bound_ != '':
                file_.write('\\defUpperBound{' + variable.upper_bound_ + '}\n')
            if variable.allowed_values_ != '':
                file_.write('\\defAllowedValues{' + variable.allowed_values_ + '}\n')
            file_.write('\n')

class Latex:
    def Build(self):

        print '-- Building latex documentation and pdf'
        cwd = os.path.normpath(os.getcwd())
        os.chdir('../Documentation/UserManual/')
        print '-- Building CASAL.syn'
        os.system('python QuickReference.py')
        
        # Build the Version.tex file
        if Globals.git_path_ != '':
            print '-- Build version.tex with Git log information'
            p = subprocess.Popen(['git', '--no-pager', 'log', '-n', '1', '--pretty=format:%H%n%h%n%ci' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = p.communicate()
            lines = out.split('\n')
            if len(lines) != 3:
                return Globals.PrintError('Format printed by GIT did not meet expectations. Expected 3 lines but got ' + str(len(lines)))

            time_pieces = lines[2].split(' ')
            del time_pieces[-1];
            temp = ' '.join(time_pieces)
            local_time = datetime.strptime(temp, '%Y-%m-%d %H:%M:%S')
            utc_time   = local_time.replace(tzinfo=tz.tzlocal()).astimezone(tz.tzutc())

            version = '% WARNING: THIS FILE IS AUTOMATICALLY GENERATED BY doBuild documentation. DO NOT EDIT THIS FILE\n'
            version += '\\newcommand{\\SourceControlRevisionDoc}{' + lines[0] + '}\n'
            version += '\\newcommand{\\SourceControlDateDoc}{' + utc_time.strftime('%Y-%m-%d') + '}\n'
            version += '\\newcommand{\\SourceControlYearDoc}{' + utc_time.strftime('%Y') + '}\n'
            version += '\\newcommand{\\SourceControlMonthDoc}{' + utc_time.strftime('%B') + '}\n'
            version += '\\newcommand{\\SourceControlTimeDoc}{' + utc_time.strftime('%H:%M:%S') + '}\n'
            version += '\\newcommand{\\SourceControlShortVersionDoc}{' + utc_time.strftime('%Y-%m-%d') + ' (rev. ' + lines[1] + ')}\n'
            version += '\\newcommand{\\SourceControlVersionDoc}{' + utc_time.strftime('%Y-%m-%d %H:%M:%S %Z') + ' (rev. ' + lines[1] + ')}\n'
            file_output = open('Version.tex', 'w')
            file_output.write(version)
            file_output.close()
        else:
            print '-- Building a default version.tex because Git was not found'
            version = '% WARNING: THIS FILE IS AUTOMATICALLY GENERATED BY doBuild documentation. DO NOT EDIT THIS FILE\n'
            version += '\\newcommand{\\SourceControlRevisionDoc}{000000}\n'
            version += '\\newcommand{\\SourceControlDateDoc}{0000-00-00}\n'
            version += '\\newcommand{\\SourceControlYearDoc}{0000}\n'
            version += '\\newcommand{\\SourceControlMonthDoc}{00}\n'
            version += '\\newcommand{\\SourceControlTimeDoc}{00:00:00}\n'
            version += '\\newcommand{\\SourceControlShortVersionDoc}{0000-00-00 (rev. 000000)}\n'
            version += '\\newcommand{\\SourceControlVersionDoc}{0000-00-00 00:00:00 (rev. 000000)}\n'
            file_output = open('Version.tex', 'w')
            file_output.write(version)
            file_output.close()

        for i in range(0,3):
          if Globals.operating_system_ == "linux":
            if os.system('bibtex CASAL2') != EX_OK:
              return False
            if os.system('pdflatex --halt-on-error --interaction=nonstopmode CASAL2') != EX_OK:
              return False
            if os.system('makeindex CASAL2') != EX_OK:
              return False
            if not os.path.exists('CASAL2.pdf'):
              return False
          else:
            if os.system('bibtex.exe CASAL2') != EX_OK:
              return Globals.PrintError('bibtex failed')
            if os.system('pdflatex.exe --halt-on-error --enable-installer CASAL2') != EX_OK:
              return Globals.PrintError('pdflatex failed')
            if os.system('makeindex.exe CASAL2') != EX_OK:
              return Globals.PrintError('makeindex failed')
        print '-- Built the Casal2 usermanual'

        os.chdir('../GettingStartedGuide/')
        for i in range(0,3):
          if Globals.operating_system_ == "linux":
            if os.system('bibtex GettingStartedGuide') != EX_OK:
              return False
            if os.system('pdflatex --halt-on-error --interaction=nonstopmode GettingStartedGuide') != EX_OK:
              return False
            if os.system('makeindex GettingStartedGuide') != EX_OK:
              return False
            if not os.path.exists('GettingStartedGuide.pdf'):
              return False
          else:
            if os.system('bibtex.exe GettingStartedGuide') != EX_OK:
              return Globals.PrintError('bibtex failed')
            if os.system('pdflatex.exe --halt-on-error --enable-installer GettingStartedGuide') != EX_OK:
              return Globals.PrintError('pdflatex failed')
            if os.system('makeindex.exe GettingStartedGuide') != EX_OK:
              return Globals.PrintError('makeindex failed')
              
        print '-- Built the GettingStartedGuide'
        os.chdir('../ContributorsManual/')
        for i in range(0,3):
          if Globals.operating_system_ == "linux":
            if os.system('pdflatex --halt-on-error --interaction=nonstopmode ContributorsGuide') != EX_OK:
              return False
          else:
            if os.system('pdflatex.exe --halt-on-error --enable-installer ContributorsGuide') != EX_OK:
              return Globals.PrintError('pdflatex failed')
        print '-- Built the ContributorsGuide'

        return True