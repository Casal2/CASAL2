# Configuration File Syntax
Casal2 uses a human readable model definition language (MDL) to define models. Models are specified in text files using either Windows or Unix line endings and UTF-8 encoding. By default, Casal2 will load a `config.csl2` file from the current directory when executed.

The `config.csl2` file is expected to be the entry point for your model definition.

The Casal2 MDL is derived from the Spatial Population Model (SPM) one, but enhanced to make writing complex models easier. There is no direct link between Casal and Casal2 with respect to the configuration syntax.

Casal2's MDL uses a `block`, `parameter` and `command` notation. All lines within the configuration files must be either a block, parameter or command. 

An example of a defined object (e.g., a process) in Casal2 would look like:
```none
@block block_label
parameter value
parameter value1 value2
```

Some general notes about writing configuration files:
1. Whitespace can be used mostly-freely. Tabs and spaces are both accepted
2. A block ends only at the beginning of a new block or end of the final configuration file
3. You can include another file from anywhere
4. Included files are placed inline, so you can continue a block in a new file
5. The configuration files support inline declarations of objects

# Basic Concepts
## The block
The block is a collection of parameters that define a single object within your model.

Your model is created by having many blocks that define how the model is to be built. All objects with Casal2 are presented through blocks in the configuration file. Examples of some common blocks are:
```none
@model
@categories
@time_step label
@process label
@observation label
@likelihood label
@estimate label
@selectivity label
```

Blocks may or may not require a label depending on their function. As a general rule, a block requires a label if you can specify more than one of that block type. You can only specify one `@model` block for your model, therefore it does not require a label. As you can specify many `@process` objects in your model, they do require a label.

## The parameter
Following a block definition, the parameters for that object are specified. Parameters are specified one per line with the first value on the line being the name of the parameter you wish to define.

Example parameters would be:
```none
@model
start_year 1999
final_year 2010

@categories
format sex
names male female

@process ageing
type ageing
categories male female
```

## A command
Commands are used to tell Casal2 to perform an action while loading the configuration file. At present, Casal2 only supports a single command. This command includes another file inline from where the command is called. This allows you to spread your configuration across many files including parts of the configuration where required.

Importantly, the included file is essentially inserted in to the calling file where the include is called from. This allows you to spread the definition of a single block across multiple files, allowing for the parameters to be specified with the block definition, while the data is stored in a separate file possibly automatically generated.

_Note: There are no restrictions on the name of the file, but we do recommend avoiding spaces in the name and paths._

The including of files is done by:
```none
@process my_made_up_process
type mortality_with_table_of_m
categories *
!include “mortality_table_data.csl2”

@block next_process
Type ageing
```

# Advanced Concepts

## Addressables
Within Casal2 many parameters are registered as `addressable`. This is a mechanism by which Casal2 makes the parameter addressable from a configuration file. Put simply, it allows you to use the reference a parameter on another block and use it. This is most commonly used for defining which parameters in the model will be estimated (free parameters).

The syntax for an addressable is `block[label].parameter(index)` where index is optional for addressing lists/vectors or maps.

An example of using an addressable to estimate r0 on a recruitment process would look like:
```none
@process my_recruitment
type recruitment_constant
r0 9999

@estimate my_estimate
parameter process[recruitment_constant].r0
```

## Inline Declarations
Inline declarations allow you to define an entire object in a single configuration line. This is useful for defining an object that will only be used by one other object. Casal2 will create an object in memory with an addressable label for use by the model.

An example of an inline declaration is:
```none
@process my_recruitment
type recruitment_constant
r0 999
categories male 
selectivities [type=constant; c=1]
```

The above block definition inline declares a `selectivity` of type`constant` with the parameter `c` set to `1`. 

# Keywords and Reserved Characters
The following are reserved characters in the Casal2 configuration file. They cannot bs used for labels or parameter names.

## @ Block Definition
Every block in the configuration file must start with the block definition character. The reserved character for this is the `@` character.

## 'type' Keyworld
The `type` keyword is used to specify the object sub-type when after specifying the parent type in the block definition.

Example:
```none
@process my_process # process is parent type
type recruitment_constant # type is reserved and recruitment_constant is sub-type
```

## '#' Single line comment
When writing configuration files, you can use comments. Comments will be ignored by the Casal2 configuration loader.

Example:
```none
@block <label>
type <sub_type> #Descriptive comment
#parameter <value_1> – This whole line is commented out
parameter <value_1> #<value_2>(value_2 is commented out)
```

## '/*' and '*/' Multi-line comment
Like the C++ programming language, Casal2 uses `/*` and `*/` to start and end multi-line comment blocks. 

Example:
```none
@block <label>
type <sub_type>
parameter <value_1>
parameter <value_1> <value_2>

/* Do not load this process
@block <label>
type <sub_type>
parameter <value_1>
parameter <value_1> <value_2>
*/
```

## '*' Wildcard Operator
When referencing categories, Casal2 supports the use of the wildcard operator to select all
categories. It’s important to note that the wildcard operator will preserve the order of definition of
category names.

Example:
```none
@categories
format sex.stage
names male.immature male.mature female.immature female.mature

@process my_recruitment_process
type constant_recruitment
categories * #Will select all categories defined preserving order
```

## ':' Range Operator
The range specifier allows you to specify a range of values at once instead of having to input them manually. Ranges can be either incremental or decremental.

```none
@process my_recruitment_process
type constant_recruitment
years_to_run 2000:2007 #With range specifier

@process my_mortality_process
type natural_mortality
years_to_run 2000 2001 2002 2003 2004 2005 2006 2007 #Without range specifier
```

## ',' List Operator
When a parameter supports multiple values in a single entry you can use the list specifier to supply multiple values as a single parameter.

```none
@categories
format sex.stage
names male,female.immature,mature #With list specifier

@categories
format sex.stage
#Without list specifier
names male.immature male.mature female.immature female.mature
```

## '+' Join Operator
Some observation blocks allow the joining of multiple categories into a single collection for processing. Casal2 supports the use of a join operator for observations that support this.

```none
@observation
type joinable
categories male+female female # will show as 2 collections to observation
```