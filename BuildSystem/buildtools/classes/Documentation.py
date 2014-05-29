import os
import sys
import subprocess
import os.path
import fileinput
import re

import Globals

EX_OK = getattr(os, "EX_OK", 0)

class Documentation:
    # Members
    current_output_file_ = ''
    parent_file_ = ''
    parent_class_ = ''
    variables_ = {}

    variable_type_ = {}
    variable_label_ = {}
    variable_description_ = {}
    variable_value_ = {}
    variable_default_ = {}

    translations_ = {}

    # Methods
    def __init__(self):
        print '--> Starting Documentation Builder'

    def clean_variables(self):
        self.variable_type_ = {}
        self.variable_label_ = {}
        self.variable_description_ = {}
        self.variable_value_ = {}
        self.variable_default_ = {}        
                                 
    def start(self):
        self.load_translations()
        folder_list = [ 'AgeingErrors', 'AgeSizes', 'Catchabilities', 'DerivedQuantities',
                        'Estimates', 'InitialisationPhases', 'Likelihoods', 'MCMC', 'Minimisers',
                        'Model', 'Observations', 'Penalties', 'Processes', 'Profiles', 'Reports',
                        'Selectivities', 'SizeWeights', 'TimeSteps']        
        for folder in folder_list:
            self.clean_variables()            
            file_list = os.listdir('../iSAM/source/' + folder + '/')
            for file in file_list:                
                if file.startswith(folder[:-3]) and file.endswith('.h'):
                    self.parent_file_ = file
                    print '--> Parent File: ' + self.parent_file_
                    class_name = self.parent_file_.replace('.h', '')
                    self.parent_class_ = class_name
                    self.load_variables(class_name, '../iSAM/source/' + folder + '/' + self.parent_file_)
                    self.print_parent('../iSAM/source/' + folder + '/' + self.parent_file_)
                    self.clean_variables()
                    break

            if os.path.exists('../iSAM/source/' + folder + '/Children'):
                file_list = os.listdir('../iSAM/source/' + folder + '/Children/')
                for file in file_list:
                    if (file.endswith(".h")):
                        print '--> Child File: ' + file
                        class_name = file.replace('.h', '')
                        self.load_variables(class_name, '../iSAM/source/' + folder + '/Children/' + file)
                        self.print_child(file.replace('.h', ''), '../iSAM/source/' + folder + '/Children/' + file)
                        self.clean_variables()

    def load_translations(self):
        print '--> Loading translations'
        file = fileinput.FileInput('../iSAM/source/Translations/English_UK.h')
        for line in file:
            if not line.startswith('#define'):
                continue
            pieces = line.split()
            if len(pieces) < 3 or not pieces[1].startswith('PARAM'):
                continue

            pieces = re.split('"', line)
            if len(pieces) != 3:
                print '--> Error parsing translation line: ' + line
                return

            temp = pieces[0].split()
            if len(temp) != 2:
                print '--> Error parsing translation line piece: ' + pieces[0]
                return

            lookup = temp[1]
            value = pieces[1]
            self.translations_[lookup] = value.replace('_', '\_')         
        
    def load_variables(self, class_name, header_file):
        print '--> Loading variables: ' + header_file
        start_processing = False;
        class_name = class_name.lower()
        print '--> Class: ' + class_name
        
        fi = fileinput.FileInput(header_file)
        for line in fi:
            line = line.lower().rstrip().lstrip()
            if line.startswith('class ' + class_name):
                print '--> Processing can begin: ' + class_name
                start_processing = True
                continue
            if not start_processing:
                continue
            if 'virtual' in line:
                continue
            if '(' in line:
                continue
            if line == '};':
                start_processing = False
                continue;
            if line == '' or line.startswith('//'):
                continue
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
            print '--> Storing variable: ' + pieces[1]
            self.variables_[pieces[1]] = pieces[0]

        source_file = header_file.replace('.h', '.cpp')
        print '--> Loading from .cpp - ' + source_file
        start_processing = False
        fi = fileinput.FileInput(source_file)
        for line in fi:
            line = line.rstrip().lstrip()
            lower_line = line.lower().rstrip().lstrip()
            if not start_processing and lower_line.startswith(class_name + '::' + class_name + '('):
                print '--> Starting process'
                start_processing = True
                continue
            if not start_processing or line == '\n':
                continue
            if line == '}' or line == '}\n':
                start_processing = False
                continue
            if not line.startswith('parameters_.Bind<'):
                continue
            short_line = line.replace('parameters_.Bind<', '')
            pieces = re.split(',|<|>|;|(|)', short_line)
            if len(pieces) < 5:
                print '--> Error parsing line: ' + line
                continue

            pieces = filter(None, pieces)            
            name = pieces[1].replace('(', '')
            used_variable = pieces[2].replace('&', '').rstrip().lstrip()         
            if used_variable in self.variables_ and self.variables_[used_variable] != pieces[0]:
                self.variable_type_[name] = self.variables_[pieces[1].replace('&', '')]
            else:
                self.variable_type_[name] = pieces[0]
            self.variable_label_[name] = name
            self.variable_description_[name] = pieces[3].replace('"', '').rstrip().lstrip()
            self.variable_value_[name] = pieces[4].replace('"', '').replace(')', '').rstrip().lstrip()
            if len(pieces) == 6:
                self.variable_default_[name] = pieces[5].replace(')', '').rstrip().lstrip()
            else:
                self.variable_default_[name] = 'No default'
            print '--> Loaded variable: ' + name
            

    def print_parent(self, header_file):        
        self.current_output_file_ = 'Documentation/' + self.parent_file_.replace('.h', '.tex')
        source_file = header_file.replace('.h', '.cpp')
        print '--> Printing parent latex from ' + source_file + ' to ' + self.current_output_file_

        class_name = self.parent_file_.replace('.h', '')
        if class_name != 'MCMC':
            class_name = re.sub( '(?<!^)(?=[A-Z])', '\_', class_name)

        if not os.path.exists('Documentation/'):
            os.makedirs('Documentation/')
        
        file = open(self.current_output_file_, 'w')
        file.write('\defComLab{' + class_name.lower() + '}{Define an object type ' + class_name + '}\n')
        file.write('\n')

        for name in self.variable_label_:
            file.write('\\defSub{' + self.translations_[name] + '} {' + self.variable_description_[name] + '}\n')
            file.write('\\defType{' + self.variable_type_[name] + '}\n')
            if self.variable_default_[name].startswith('PARAM_'):
                file.write('\\defDefault{' + self.translations_[self.variable_default_[name]] + '}\n')
            else:
                file.write('\\defDefault{' + self.variable_default_[name] + '}\n')
            if name in self.variable_value_ and self.variable_value_[name] != '':
                file.write('\\defValue{' + self.variable_value_[name] + '}\n')
            file.write('\n')
        
        file.close()

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
            file.write('\\defSub{' + self.translations_[name] + '} {' + self.variable_description_[name].replace('_', '\_') + '}\n')
            file.write('\\defType{' + self.variable_type_[name] + '}\n')
            if self.variable_default_[name].startswith('PARAM_'):
                file.write('\\defDefault{' + self.translations_[self.variable_default_[name]] + '}\n')
            else:
                file.write('\\defDefault{' + self.variable_default_[name] + '}\n')
            if name in self.variable_value_ and self.variable_value_[name] != '':
                file.write('\\defValue{' + self.variable_value_[name] + '}\n')
            file.write('\n')
        
        file.close()
            
            
    
        

    
