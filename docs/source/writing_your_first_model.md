# Writing Your First Model
Below, we will cover writing your first complete model in Casal2. We will be writing a "Two Sex" model where the population has two defined sexes (male and female), some basic processes. 

By the end of this process, you will have a model that can do a basic execution using Casal2. Adding to this model and introducing estimates and observations will be done int he next section `intermediate concepts`.

# Creating your configuration file
Assuming that you have followed the installation instructions and have Casal2 working, you will need to create your configuration file. Create a file named `config.csl2` in the current directory. This file should be a basic text file with UTF-8 encoding. Line endings can be either Windows or Linux.

# Specifying the Model object
The first block that must be defined in every configuration is the `@model` block. This defines the key attributes for our model. 

For our model, we're going to specify the following parameters in the configuration file:
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
What we're specifying above is a model that will run from `1994` to `2008`. It has a population with ages `1` to `50`. The specification of `age_plus` means that population members aged `50` will not be removed from the population when they are aged again, but will remain at age `50` until they are removed through another means (e.g., mortality). We are specifying that we would like to run a single initialisation phase named `iphase1` that we haven't defined yet, and two time steps per year named `step_one` and `step_two` both of which we haven't yet defined.

Importantly, the labels of objects like the initialiation phases and time steps defined here are arbitrary and can be what you want them to be. If you have a model with 4 time steps per year, then you could name them `autumn`, `summer`, `winter`, `spring`. They are user defined.

# Specifying the Categories for our model
Now that we have specified our model information, we need to provide some more high level information. The `@categories` block is good to define next, but can be defined anywhere within your configuration file.

When we define categories we need to tell Casal2 the format for our category labels, and the category labels we want created. Category labels have a hierarchy that allows you to represent custom concepts (e.g., tagging or spatial dynamics). For this model, we're doing to specify two sexes, but each sex (male, female) will have two maturities (immature, mature). 

Add the following definition to your configuration below the `@model` block:
```none
@categories
format stage.sex
names immature.male mature.male immature.female mature.female
```

You should consider the order of your categories as important. When using short-hand lookups of categories, Casal2 will always preserve the order that you specify in the `@categories` block. 

# Initialisation and Time Steps
Next we want to specify our intialisation and time step phases. In our model block we specified one initialisation phase `iphase1`, so we'll handle that first.
Add the following to your configuration file:

```none
@initialisation_phase iphase1
years 200
```

This is a super simple definition that tells Casal2 that we want to run our annual cycle (the time steps we specify) 200 times before we start our model run. This allows us to initialise the population to an equilibrium state before we execute our model. It'd make no sense to start with an empty population.

Next, we want to specify our annual cycle. We specified two time steps in our model block `step_one` and `step_two`. This means our annual cycle is broken in to two discrete parts. You can specify as many time steps as you want, but you must have at least one time step for a model to run.

Let's add the following time steps to our model:
```none
@time_step step_one
processes recruitment maturation mortality

@time_step step_two
processes ageing
```

Looking above, you may ask why have we put the ageing process into it's own time step? Casal2 allows you to report or observe on a time step, so you will often break your annual cycle up to match the reporting and observation data that you require. The above example, allows us to observe or report on the population right after we have completed our mortality process.

Again, the labels specified for each process is user defined. For this tutorial, we have decided to keep them simple and name our processes `recruitment`, `maturation`, `mortality` and `ageing`. You can name them whatever is suitable for your environment.

# Checkpoint
Right, let's check in to see what our configuration file should look like in totality. We've defined a `model`, `categories`, `initialisation_phase` and `time_steps`. This gives us the basic structure of the model we want to run, but we haven't yet added the details.

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

As you can see, Casal2's syntax is very easy to both read and write. Now we'll continue to finish our model by defining our processes and a simple report to view the partition once the model completes execution.

# Recruitment Process
In our model, we have defined a process named `recruitment`. While the name indicates the type of process, this is just a useful naming scheme that we're using. It could be called anything.. including `banana`. Casal2 does not enforce naming of objects because it is interested in parsing your model configuration and building the model relationships before execution.

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

You will notice some interesting use of model definition language (MDL) shorthand when specifying the categories. When we defined our categories we defined a format of `stage.sex` and we're using the shorthand to only get the categories where the `stage` is `immature`. 

We also specify two proportions, knowing that we only have two categories defined with the `stage=immature` and Casal2 will validate this for us as part of executing the model. 

# Maturation Process
While we're calling our process `maturation`, in Casal2 it is considered a category shift or transition. That is, we're telling Casal2 to move fish from one category to another in a specific way. For this model, we're going to move older members of the population from `immature` to `mature` using a category transition. We'll also be using a selectivity to influence the numbers of fish we take at each age. The older a fish is, the more likely it is to mature.

So, we will define a new `process` called `maturation` that will move fish from `stage=immature` to `stage=mature` in equal proportions for the sexes based on a logistic producing selectivity.

Add the following process definition to your configuration file:
```none
@process maturation
type transition_category
from stage=immature
to stage=mature
proportions 1.0 1.0
selectivities maturation_sel maturation_sel
```

Note that we've specified the name of a selectivity `maturation_sel` that we haven't yet defined. We will define this later in our model.

# Mortality Process
Our mortality process represents a class of processes that remove members of the population. For this tutorial we're specifying a `natural mortality` process that will remove `0.065` percent of the population split across the ages using a selectivity defined as `relative_m_by_age` and at the end of the `time_step`.

We'll specify `1.0` for `time_step_proportions` and ignore this. Time step proportions are used when you call the same process multiple times during an annual cycle and want to take a different proportion of `m` in each `time step`. We're only calling this process once in a single time step, so we'll set it to `1.0`.

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

# Ageing Process
Our ageing process is a very simply process to age the population by one year every time it is called.

Add the following definition to your configuration file:
```none
@process ageing
type ageing
categories *
```

# Selectivities
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

# Run the model
At this point we have a working configuration file. If you run `casal2 -r` you should see Casal2 execute and complete the model run with no errors. You will also notice, that you get no outputs. Casal2 by default does not produce any outputs, and relies on you to specify the outputs you would like as part of your model definition.

Model outputs in Casal2 are defined using `reports`. We'll quickly add a `partition` report to our model and re-run it. Add the following to your configuration file:
```none
@report partition
type partition
years 2008
time_step step_two
```