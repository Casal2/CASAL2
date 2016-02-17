import os
import sys
import subprocess
import os.path
import fileinput
import re
from datetime import datetime, date
from dateutil import tz

import Globals

EX_OK = getattr(os, "EX_OK", 0)

class Documentation:
    # Members
    current_output_file_ = ''
    build_latex_ = False
    parent_file_ = ''
    parent_class_ = ''
    variables_ = {}
    estimables_ = {}
    child_directories_ = []
   
    variable_type_ = {}
    variable_label_ = {}
    variable_description_ = {}
    variable_value_ = {}
    variable_default_ = {}
    variable_lower_bound_ = {}
    variable_upper_bound_ = {}
    variable_allowed_values_ = {}

    type_translations_ = {}
    translations_ = {}

    # Methods
    def __init__(self):
        print '--> Starting Documentation Builder'
        self.type_translations_['double']           = 'constant'
        self.type_translations_['Double']           = 'constant'
        self.type_translations_['vector<double>']   = 'constant vector'
        self.type_translations_['vector<Double>']   = 'constant vector'
        self.type_translations_['unsigned']         = 'non-negative integer'
        self.type_translations_['vector<unsigned>'] = 'non-negative integer vector'
        self.type_translations_['vector<string>']   = 'string vector'
        self.type_translations_['bool']             = 'boolean'
        self.type_translations_['vector<bool>']     = 'boolean vector'
        self.type_translations_['map<string, vector<Double>>'] = 'constant vector'

    """
    Start the documentation builder
    """
    def start(self):
        self.clean_variables()
        # Figure out if we have Latex in our path, if we do then we want to
        # build the PDF documentation as well
        if Globals.latex_path_ != "":
            self.build_latex_ = True
        # Load our translations from the Translations.h file so we can convert
        # PARAM_X in to actual English
        if not self.load_translations():
            return False
        folder_list = [ 'AdditionalPriors', 'AgeingErrors', 'AgeLengths', 'Asserts',
                        'Catchabilities', 'Categories', 'DerivedQuantities',
                        'Estimates', 'InitialisationPhases', 'LengthWeights',
                        'Likelihoods', 'MCMCs', 'Minimisers',
                        'Model', 'Observations', 'Penalties', 'Processes', 'Profiles', 'Projects',
                        'Reports', 'Selectivities', 'Simulates', 'TimeSteps', 'TimeVarying']
        for folder in folder_list:
            self.clean_variables()            
            file_list = os.listdir('../CASAL2/source/' + folder + '/')
            for file in file_list:                
                if file.startswith(folder[:-3]) and file.endswith('.h') and not file.endswith('-inl.h'):
                    self.parent_file_ = file
                    print '--> Parent File: ' + self.parent_file_
                    class_name = self.parent_file_.replace('.h', '')
                    self.parent_class_ = class_name
                    if not self.load_variables(class_name, '../CASAL2/source/' + folder + '/' + self.parent_file_):
                        return False
                    if not self.print_parent('../CASAL2/source/' + folder + '/' + self.parent_file_):
                        return False
                    self.clean_variables()
                    break
            
            if os.path.exists('../CASAL2/source/' + folder + '/Children'):
                file_list = os.listdir('../CASAL2/source/' + folder + '/Children/')
                self.child_directories_ = []
                for file in file_list:
                    full_file = '../CASAL2/source/' + folder + '/Children/' + file
                    if os.path.isdir(full_file):
                        self.child_directories_.append(file)
                        
                for file in file_list:                    
                    if (file.endswith(".h")):
                        print '--> Child File: ' + file
                        class_name = file.replace('.h', '')
                        for directory in self.child_directories_:
                            print '-- Dir: ' + directory
                            if class_name.endswith(directory) and os.path.exists('../CASAL2/source/' + folder + '/Children/' + directory + '/' + directory + '.h'):
                              print 'X'
                              if not self.load_variables(directory, '../CASAL2/source/' + folder + '/Children/' + directory + '/' + directory + '.h'):
                                return False
                              if not self.print_child(file.replace('.h', ''), '../CASAL2/source/' + folder + '/Children/' + class_name + '/' + class_name):
                                return False                        
                        if not self.load_variables(class_name, '../CASAL2/source/' + folder + '/Children/' + file):
                            return False
                        if not self.print_child(file.replace('.h', ''), '../CASAL2/source/' + folder + '/Children/' + file):
                            return False
                        self.clean_variables()

        self.variable_type_ = {}
        self.variables_ = {}    
        if self.build_latex_:
            self.build_latex()
        
        return True

    """
    Clear all of our variables
    """
    def clean_variables(self):        
        self.variable_label_ = {}
        self.variable_description_ = {}
        self.variable_value_ = {}
        self.variable_default_ = {}
        self.estimables_ = {}
        self.variable_lower_bound_ = {}
        self.variable_upper_bound_ = {}
        

    """
    Load the translations from our translation file. This informaiton gets stored in a map
    so it can be used for converting the PARAM_X names in to English when we print the details
    to the file
    """
    def load_translations(self):
        print '--> Loading translations'
        file = fileinput.FileInput('../CASAL2/source/Translations/English_UK.h')
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
            self.translations_[lookup] = value            
        return True

    """
     Load the variables from the specified class name and header file
    """
    def load_variables(self, class_name, header_file):
        print '--> Loading variables: ' + header_file
        start_processing = False;
        class_name = class_name.lower()
        print '--> Class: ' + class_name
        self.variables_['label_'] = 'string'
        self.variables_['type_'] = 'string'

        # First we load the header file so we can find the declaratons
        # for the variables. This will give us a better view of the type of
        # variable when we need to determine if the variable is a vector        
        # or just a standard variable
        class_count = 0;
        fi = fileinput.FileInput(header_file)
        for line in fi:
            line = line.lower().rstrip().lstrip()
            # Try to find the start of the class definition so we can actually start processing
            if line.startswith('class ' + class_name):      
            	class_count += 1      	
                start_processing = True
                continue
            if line.startswith('struct'):
            	print 'Struct found: ' + line
            	class_count += 1
            	continue
            if not start_processing:
                continue
            # skip any declarations that will be methods
            if 'virtual' in line:
                continue
            if '(' in line:
                continue
            if line == '};':
            	class_count -= 1
            	if class_count == 0:
                	start_processing = False
                continue;
            if line == '' or line.startswith('//'):
                continue
            # Skup C++ keywords
            if line == 'public:' or line == 'protected:' or line == 'private':
                continue

            # Shrink any gaps we have have in the declarion of the type
            # e.g vector< vector<string> >
            line = line.replace('< ', '<')
            line = line.replace('> >', '>>')
            line = line.replace(', ', ',')
            
            pieces = line.split();
            if len(pieces) < 2:
                continue
            variable_type = pieces[0]
            variable_name = pieces[1].replace(';', '')
            print '--> Storing variable: ' + variable_name + ' as type ' + variable_type
            self.variables_[variable_name] = variable_type

        # Now we've finished with the header file we start to work with the source (.cpp) file
        # We want to only work in the constructor because that is where everything we want is stored
        source_file = header_file.replace('.h', '.cpp')
        print '--> Loading from .cpp - ' + source_file
        start_processing = False
        continue_line = False
        last_line = ''
        fi = fileinput.FileInput(source_file)
        for line in fi:
            line = line.rstrip().lstrip()
            lower_line = line.lower().rstrip().lstrip()
            # look specifically for the constructor then start processing
            if not start_processing and lower_line.startswith(class_name + '::' + class_name + '('):
                print '--> Starting process'
                start_processing = True
                continue
            if not start_processing or line == '\n':
                continue
            # Stop processing after a single line with } as this is the end of the method
            if line == '}' or line == '}\n':
                break

            # Only process lines with the parameters_.Bind< or RegisterEstimable(
            if continue_line:
                line = last_line + line
                last_line = line
            if line.endswith(';'):
                continue_line = False                
                last_line = ''
                
            if line.startswith('parameters_.Bind<') and line.endswith(';'):                
                if not self.load_parameter_bind(line):
                    return False
            elif line.startswith('RegisterAsEstimable(') and line.endswith(';'):
                if not self.load_register_estimable(line):
                    return False
            elif not line.endswith(';'):                
                last_line = last_line + line
                continue_line = True
                
        return True
            
    def load_parameter_bind(self, line):
        lines  = re.split('->', line)
        short_line = lines[0].replace('parameters_.Bind<', '')        
        pieces = re.split(',|<|>|;|(|)', short_line)
        pieces = filter(None, pieces)
        # Check for Name            
        name = pieces[1].replace('(', '').rstrip().lstrip()
        self.variable_label_[name] = name
        print '--> Name: ' + name            
        
        # Check for the name of the variable we're binding too
        used_variable = pieces[2].replace('&', '').rstrip().lstrip().lower()
        print '-- Variable: ' + used_variable
        
        # Get the variable type            
        if used_variable in self.variables_:
            self.variable_type_[name] = self.variables_[used_variable]
        else:
            return Globals.PrintError('Could not set type for variable: ' + name + ' using variable ' + used_variable)
        print '-- Type: ' + self.variable_type_[name]

        # Set the description
        index = 3;
        description = pieces[index]
        while description.count('"') != 2:
            index += 1
            description += ',' + pieces[index]
        self.variable_description_[name] = description.replace('R"(', '').replace(')"', '').replace('"', '').rstrip().lstrip()
        print '-- Description: ' + self.variable_description_[name]            

        # Set the value
        index += 1
        value = pieces[index]
        while value.count('"') != 2:
            index += 1
            value += ',' + pieces[index]
        value = value.replace(')', '')
        self.variable_value_[name] = value.replace('R"(', '').replace(')"', '').replace('"', '').replace(')', '').rstrip().lstrip()
        print '-- Value: ' + self.variable_value_[name]            
        
        # Set the default value
        index += 1
        if len(pieces) > index:
            self.variable_default_[name] = pieces[index].replace(')', '').rstrip().lstrip()
        else:
            self.variable_default_[name] = 'No default'
        print '-- Default: ' + self.variable_default_[name]

        if len(lines) == 2:
            short_line = lines[1]
            if short_line.startswith('set_lower_bound'):
                lower_bound_info = short_line.replace('set_lower_bound(', '').replace(');', '').split(',')                
                lower_bound = lower_bound_info[0]
                if len(lower_bound_info) == 2 and lower_bound_info[1].lstrip().rstrip().lower() == 'false':
                    lower_bound += ' (exclusive)'                        
                else:
                    lower_bound += ' (inclusive)'                        
                self.variable_lower_bound_[name] = lower_bound
                print '-- Lower Bound: ' + lower_bound
            elif short_line.startswith('set_upper_bound'):
                upper_bound_info = short_line.replace('set_upper_bound(', '').replace(');', '').split(',')                
                upper_bound = upper_bound_info[0]
                if len(upper_bound_info) == 2 and upper_bound_info[1].lstrip().rstrip().lower() == 'false':
                    upper_bound += ' (exclusive)'                        
                else:
                    upper_bound += ' (inclusive)'                        
                self.variable_upper_bound_[name] = upper_bound
                print '-- Upper Bound: ' + upper_bound
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
                self.variable_lower_bound_[name] = lower_bound
                print '-- Lower Bound: ' + lower_bound
                self.variable_upper_bound_[name] = upper_bound
                print '-- Upper Bound: ' + upper_bound
            elif short_line.startswith('set_allowed_values('):
                info = short_line.replace('set_allowed_values({', '').replace('});', '').replace('"', '').split(',')
                allowed_values = []
                for value in info:
                    value = value.strip().lstrip()
                    if value.startswith('PARAM'):
                        allowed_values.append(self.translations_[value])
                    else:
                        allowed_values.append(value)
                short_line = ', '.join(allowed_values)
                self.variable_allowed_values_[name] = short_line
                print '-- Allowed Values: ' + short_line                
        
        return True
    
    """
    This method will take the RegisterAsEstimable() line and record what variable
    is an estimate so we can specify this in the documentation
    """
    def load_register_estimable(self, line):
        short_line = line.replace('RegisterAsEstimable(', '')
        pieces = re.split(',|<|>|;|(|)', short_line)
        pieces = filter(None, pieces)

        if len(pieces) != 2 and len(pieces) != 1:
            return Globals.PrintError('Expected 2 or 3 pieces but got ' + str(len(pieces)) + ' with line: ' + line)

        if len(pieces) == 1:
          name = 'user defined'
          variable = pieces[0].replace('&', '').replace(')', '').lstrip().rstrip()
        else:
          name = pieces[0]
          variable = pieces[1].replace('&', '').replace(')', '').lstrip().rstrip()

        print '--> Estimable: ' + name + ' with variable ' + variable
        self.estimables_[name] = variable
        return True

    """
    """
    def print_parent(self, header_file):        
        self.current_output_file_ = '../Documentation/Manual/Syntax/' + self.parent_file_.replace('.h', '.tex')
        source_file = header_file.replace('.h', '.cpp')
        print '--> Printing parent latex from ' + source_file + ' to ' + self.current_output_file_

        class_name = self.parent_file_.replace('.h', '')
        if class_name != 'MCMC':
            class_name = re.sub( '(?<!^)(?=[A-Z])', '\_', class_name)

        if not os.path.exists('../Documentation/Manual/Syntax/'):
            os.makedirs('../Documentation/Manual/Syntax/')
        
        file = open(self.current_output_file_, 'w')
        file.write('\defComLab{' + class_name.lower() + '}{Define an object type ' + class_name + '}\n')
        file.write('\n')

        for name in self.variable_label_:
            if name not in self.translations_:
                return Globals.PrintError('Name ' + name + ' not found in translations')
            file.write('\\defSub{' + self.translations_[name] + '} {' + self.variable_description_[name] + '}\n')

            if name in self.estimables_:
                if self.variable_type_[name].startswith('vector<'):
                    file.write('\\defType{estimable vector}\n')
                else:
                    file.write('\\defType{estimable}\n')
            elif self.variable_type_[name] in self.type_translations_:
                file.write('\\defType{' + self.type_translations_[self.variable_type_[name]] + '}\n')
            else:
                file.write('\\defType{' + self.variable_type_[name] + '}\n')
            if self.variable_default_[name].startswith('PARAM_'):
                file.write('\\defDefault{' + self.translations_[self.variable_default_[name]] + '}\n')
            else:
                file.write('\\defDefault{' + self.variable_default_[name] + '}\n')
            if name in self.variable_value_ and self.variable_value_[name] != '':
                file.write('\\defValue{' + self.variable_value_[name].replace('_', '\_') + '}\n')
            if name in self.variable_lower_bound_:
                file.write('\\defLowerBound{' + self.variable_lower_bound_[name] + '}\n')
            if name in self.variable_upper_bound_:
                file.write('\\defUpperBound{' + self.variable_upper_bound_[name] + '}\n')
            if name in self.variable_allowed_values_:
                file.write('\\defAllowedValues{' + self.variable_allowed_values_[name] + '}\n')        
            file.write('\n')
        
        file.close()
        return True

    """
    """
    def print_child(self, class_name, header_file):        
        source_file = header_file.replace('.h', '.cpp')
        print '--> Printing parent latex from ' + source_file + ' to ' + self.current_output_file_

        object_name = re.sub( '(?<!^)(?=[A-Z])', ' ', class_name)
        class_name = re.sub( '(?<!^)(?=[A-Z])', '\_', class_name).lower()
        class_name = class_name.replace('m\_c\_m\_c', 'mcmc')
        parent_class = re.sub( '(?<!^)(?=[A-Z])', '\_', self.parent_class_).lower()
        file = open(self.current_output_file_, 'a')
        file.write('\subsubsection[' + object_name + ']{\\commandlabsubarg{' + parent_class + '}{type}{' + class_name + '}}\n')
        file.write('\n')

        for name in self.variable_label_:
            file.write('\\defSub{' + self.translations_[name] + '} {' + self.variable_description_[name] + '}\n')

            if name in self.estimables_:
                if self.variable_type_[name].startswith('vector<'):
                    file.write('\\defType{estimable vector}\n')
                else:
                    file.write('\\defType{estimable}\n')
            elif self.variable_type_[name] in self.type_translations_:
                file.write('\\defType{' + self.type_translations_[self.variable_type_[name]] + '}\n')
            else:
                file.write('\\defType{' + self.variable_type_[name] + '}\n')
            if self.variable_default_[name].startswith('PARAM_'):
                file.write('\\defDefault{' + self.translations_[self.variable_default_[name]] + '}\n')
            else:
                file.write('\\defDefault{' + self.variable_default_[name] + '}\n')
            if name in self.variable_value_ and self.variable_value_[name] != '':
                file.write('\\defValue{' + self.variable_value_[name].replace('_', '\_') + '}\n')
            if name in self.variable_lower_bound_:
                file.write('\\defLowerBound{' + self.variable_lower_bound_[name] + '}\n')
            if name in self.variable_upper_bound_:
                file.write('\\defUpperBound{' + self.variable_upper_bound_[name] + '}\n')
            if name in self.variable_allowed_values_:
                file.write('\\defAllowedValues{' + self.variable_allowed_values_[name] + '}\n')
            file.write('\n')
        
        file.close()
        return True

    """
    """
    def build_latex(self):
        print '-- Building latex documentation and pdf'
        cwd = os.path.normpath(os.getcwd())
        os.chdir('../Documentation/Manual/')

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
            
        os.system('python QuickReference.py')
        for i in range(0,3):
          if GLobals.operating_system_ == "linux":
            os.system('pdflatex CASAL2')
            os.system('bibtex CASAL2')
            os.system('makeindex CASAL2')
          else:
            os.system('pdflatex.exe CASAL2')
            os.system('bibtex.exe CASAL2')
            os.system('makeindex.exe CASAL2')        

    
