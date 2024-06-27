# Writing Your First Model
Welcome to writing your first Casal2 model. Below, we will cover writing your first complete model in Casal2. This section covers writing the required model structure, partition definition and processes for the model to run. In the following section [](/solving_your_first_model) you will configure estimates (free parameters) and get Casal2 to solve your model using a gradient based minimiser.

This model is a basic "Two Sex" model where the population has two sexes (male and female) and basic processes for recruitment/breeding, ageing and mortality/death.

By the end of this process, you will have a model that can do a basic execution using Casal2. It will run your model through the defined annual cycles and produce a report of the partition at the end of the model. Adding to this model and introducing estimates and observations will be done in the next section [](/solving_your_first_model).

## Creating your input configuration file
When we think about what Casal2 isn't, we can say that Casal2 isn't a population dynamics model. Casal2 is a platform that allows you to define the model and it will handle building and executign that model. Casal2 is more akin to a modeling framework that allows you to build a model using a simple model definition language (MDL) rather than writing code. A typical model in Casal2 is a "no-code" model, allowing the full definition to be defined using the Casal2 model definition language and text based input configuration files.

To learn more about the Casal2 model definition and support syntaxes, we'd recommend having a read of the [](/configuration_file_syntax) page. It's not necessary for this tutorial, but would be helpful.

Assuming that you have followed the [](/installation) instructions and have Casal2 working, you will need to create your initial input configuration file. Create a file named `config.csl2` in the current directory. This file should be a basic text file with UTF-8 encoding. Line endings can be either Windows or Linux. By default, Casal2 will load the file `config.csl2` as the entry point to your model definition. For small models, you can keep everything in a single file, but for larger files it is recommended to use multiple files.

## Specifying the Model object
The first block that must be defined in every model definition is the `@model` block. This defines the key attributes for our model. Casal2 will produce an error if you do not define the `@model` block first.

For this model, we're going to specify the following parameters in the configuration file:
```none
@model
start_year 1994
final_year 2008
min_age 1
max_age 50
age_plus t
initialisation_phases iphase1
time_steps step_one step_two
```
What we're specifying above is a model that will run from years `1994` to `2008`. It has a population with ages `1` to `50`. The specification of `age_plus` means that population members aged `50` will not be removed from the population when they are aged, but will remain at age `50` until they are removed through another means (e.g., mortality/death). We are specifying that we would like to run a single initialisation phase named `iphase1` that we haven't defined yet, and two time steps per year named `step_one` and `step_two` both of which we haven't yet defined.

_Note: The initialisation process and annual cycle are completely defined by you the user as part of the model definition. In this model we have two time_steps that are executed per year in the annual cycle, but this could be as many as you want. Four for seasons, 12 for months or 22 just because. Casal2 does not enforce restrictions on this._

_Note2: the labels of objects for initialiation phases and time steps defined here are arbitrary and can be what you want them to be. If you have a model with 4 time steps per year, then you could name them `autumn`, `summer`, `winter`, `spring`. They are user defined and Casal2 does not correlate them to any internal processes/systems/constraints._

## Specifying the Categories for our model
Once we have finished our `@model` block, we can specify any other block in our model next. There is no requirement to place them in a specific order. A good convention is to next define the blocks that relate to the model partition structure and annual cycle. We will define the  `@categories` block next.

Casal2 does not have an understanding of categories as something that relates specifically to population dynamics modelling, therefore you can name and structure your categories pretty freely. But, as part of this we need to give Casal2 some information regarding the structure of our categories in terms of hierarchy. Category hierarchy in Casal2 allows us to use short-hand lookups and organise categories in a way that is easy to reference from other places within the configuration file.

The Casal2 category hierarchy means that categories can have different layers separated by the `.` character. For example, we could have categories labelled `male` and `female` each with stages of `immature` and `mature`. Casal2 doesn't natively understand that this is a concept, but we can tell it using the format parameter.

In this model, we're going to tell Casal2 that our category hierarchy has two layers, one for `stage` and one for `sex`. We do this with a `format` command. Next, we'll define the `names` of our category elements using a long form syntax. 

_Note: The order in which category names are specified is important. This order is preserved throughout the entire model. When you do a short-hand lookup of categories in a process or observation,  it will use the same order as specified here._

Add the following definition to your configuration below the `@model` block:
```none
@categories
format stage.sex
names immature.male mature.male immature.female mature.female
```

In this definition we've used the long form way of specifying the `names` of the categories. Alternatively we could've used a short form way and had the line `names immature,mature.male,female`. Because we've told Casal2 the `format` for category names in terms of hierarchy, it can help us reduce typing now and later.

## Initialisation and Time Steps
Next we want to specify our intialisation and time step phases. In the model block we specified one initialisation phase `iphase1`, so we'll handle that first.
Add the following to your configuration file:

```none
@initialisation_phase iphase1
years 200
```

This is a super simple definition that tells Casal2 that we want to run the annual cycle (the time steps we specify soon) 200 times before we start our model. This allows us to initialise the population to an equilibrium state before we execute the model. It'd make no sense to start with an empty population.

Next, we want to specify the annual cycle. We specified two time steps in the model block, `step_one` and `step_two`. This defines an annual cycle with two discrete parts. You can specify as many time steps as you want, but you must have at least one time step for a model to run.

Let's add the following time steps to the model:
```none
@time_step step_one
processes recruitment maturation mortality

@time_step step_two
processes ageing
```

Looking above, you may ask why have we put the ageing process in its own time step? Casal2 allows you to report or observe on a time step, so you will often break your annual cycle to match the reporting and observation data that you require. The above example, allows us to observe or report on the population right after we have completed our mortality process, but before ageing is executed.

Again, the labels specified for each process are user defined. For this tutorial, we have decided to keep them simple and name the processes `recruitment`, `maturation`, `mortality` and `ageing`. You can name them whatever is suitable for your environment.

## Checkpoint
Right, let's check in to see what the configuration file should look like in totality. We've defined a `model`, `categories`, `initialisation_phase` and `time_steps`. This gives us the basic structure of the model we want to run, but we haven't yet added the details.

Your configuration file should look like:
```none
@model
start_year 1994
final_year 2008
min_age 1
max_age 50
age_plus t
initialisation_phases iphase1
time_steps step_one step_two

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@time_step step_one
processes recruitment maturation mortality

@time_step step_two
processes ageing
```

As you can see, Casal2's syntax is very easy to both read and write. Now we'll continue to finish our model by defining the processes and a simple report to view the partition once the model completes.

## Recruitment Process
In the `time_step` block, we've indicated that we will have a process named `recruitment`. While the name indicates the type of process, this is just a useful naming scheme for us, Casal2 doesn't care. The process name could be called anything.. including `banana`. Casal2 does not enforce naming of objects because it doesn't assume your model is about a specific species or type of animal. It is interested in parsing your model configuration and building the model relationships before execution.

For this tutorial, we're going to add a recruitment process that adds a `constant` amount to our population. We want to add the new recruits to only the `immature` categories split evenly between `male` and `female`. 

Add the following process definition to your configuration file
```none
@process recruitment
type recruitment_constant
r0 999999
categories stage=immature
proportions 0.5 0.5
age 1
```

You will notice some interesting use of model definition language (MDL) short-hand when specifying the categories. When we defined the categories we defined a format of `stage.sex` and we're using the shorthand to only get the categories where the `stage` is `immature`. This is one of the benefits of the category hierarchy structure in Casal2.

We also specify two proportions, knowing that we only have two categories defined with the `stage=immature` and Casal2 will validate this for us as part of building the model. 

## Maturation Process
While we're calling this process `maturation`, in Casal2 it is considered a category shift or transition. That is, we're telling Casal2 to move population from one category to another in a specific way. For this model, we're going to move members of the population from `immature` to `mature` using a category transition. We'll also be using a selectivity to influence the numbers of population we take at each age. The older the population is, the more likely it is to mature.

We define a new `process` called `maturation` that will move population from `stage=immature` to `stage=mature` in equal proportions for the sexes based on a logistic producing selectivity.

Add the following process definition to your configuration file:
```none
@process maturation
type transition_category
from stage=immature
to stage=mature
proportions 1.0 1.0
selectivities maturation_sel maturation_sel
```

Again you will see the use of Casal2 model definition language short-hand syntax. Allowing us to lookup multiple categories with a simply specification, remembering that the order the categories are given to this process will be the exact same order as they were defined in the `names` paramter of the `@categories` block.

We've specified the name of a selectivity `maturation_sel` that we haven't yet defined. We will define this later in our model. A selectivity is a function that is applied to the population at each age to determine the number you want to modify. In this example, it's configured so that the older the population is, the more likely it is to be selected for the transition between categories.

## Mortality Process
The mortality process represents a class of processes that remove members of the population. For this tutorial we're specifying a `natural mortality` process that will remove `0.065` percent of the population split across the ages using a selectivity defined as `relative_m_by_age` and at the end of the `time_step`.

We'll specify `1.0` for `time_step_proportions` and ignore this. Time step proportions are used when you call the same process multiple times during an annual cycle and want to take a different proportion of `m` in each `time_step`. We're only calling this process once, so we'll set it to `1.0`.

Add the following process definition to your configuration file:
```none
@process mortality
type mortality_constant_rate
categories *
m 0.065 0.065 0.065 0.065
relative_m_by_age One One One One
time_step_proportions 1.0 
```

You will notice that we're using the wildcard operator `*` when specifying our categories. This means get all categories, in the order they were specified in our `@categories` block. It's a nice and convenient way to reference all categories.

The `relative_m_by_age` parameter also takes a label of a selectivity. In this instance we've given it the label `One` for an as-yet defined selectivity.

## Ageing Process
Our ageing process is a very simply process to age the population.

_Note: This process will age the selected population by one each time it is called. Because the annual cycle in Casal2 is user defined, it is possible to age a population multiple times in a given defined year._

Add the following definition to your configuration file:
```none
@process ageing
type ageing
categories *
```

## Selectivities
During the specification of our processes, we've referenced two different selectivities `maturation_sel` and `One`. A selectivity allows you to specify a function apply to the population that will influence the number of population members that get affected by a process or observation. 

We'll define our two selectivities by adding the following to our configuration file:
```none
@selectivity One
type constant
c 1

@selectivity maturation_sel
type logistic_producing
L 5
H 30
a50 8
ato95 3
```

## Run the model
At this point we have a working configuration file. If you run `casal2 -r` you should see Casal2 execute and complete the model run with no errors. You will also notice, that you get no outputs. Casal2 by default does not produce any outputs, and relies on you to specify the outputs you would like as part of your model definition.

Model outputs in Casal2 are defined using `reports`. We'll quickly add a `partition` report to our model and re-run it. Add the following to your configuration file:
```none
@report partition
type partition
years 2008
time_step step_two
```

Running the model now should produce a table of the population on your command line with a successful model run.

## The final model input configuration file
The following is the complete input configuration file.

```none
@model
start_year 1994
final_year 2008
min_age 1
max_age 50
age_plus t
initialisation_phases iphase1
time_steps step_one step_two

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@time_step step_one
processes recruitment maturation mortality

@time_step step_two
processes ageing

@process recruitment
type recruitment_constant
r0 999999
categories stage=immature
proportions 0.5 0.5
age 1

@process maturation
type transition_category
from stage=immature
to stage=mature
proportions 1.0 1.0
selectivities maturation_sel maturation_sel

@process mortality
type mortality_constant_rate
categories *
m 0.065 0.065 0.065 0.065
relative_m_by_age One One One One
time_step_proportions 1.0 

@process ageing
type ageing
categories *

@selectivity One
type constant
c 1

@selectivity maturation_sel
type logistic_producing
L 5
H 30
a50 8
ato95 3

@report partition
type partition
years 2008
time_step step_two
```

## Next steps
Now that we have a model that runs, we want to use Casa2 to something interesting like solving the model for unknown parameters, run a Markoc chain monte carlo (MCMC), profile a parameter, or even simulate observations of our population.

In the [](/solving_your_first_model) tutorial, we will add a solver to our model and complete an estimation run. This will require adding some observations to our model, as well as free parameters for the model to solve. 