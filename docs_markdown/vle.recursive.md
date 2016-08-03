
The package **vle.recursive** allows to perform recursive/embedded
simulations. Based on the vle API, one can launch simulations 
of a nested model into a vle Dynamics.
It provides:
* An API MetaManager for performing multi simulations
* A dynamic based on this API that can be configured by experimental conditions

The goals are:
* to provide both an API and a PDEVS dynamic for multi simulation, based on the
same approach for configuration.
* to use the same approach for configuring MPI and thread parallelization 
provided into vle. Use either the vle implementation of vle (if available)
or native threading parallelization. 
* to facilitate the stochastic simulation.
* to provide simple tools for aggregating simulation results.
* to provide an alternative to multi simulation with R, with less memory
allocation (see tests at the end).

### Multi simulation and aggregation of results

Formally a model identified by a *vpz* file allows to define multiple functions
(say *F1*, *F2*, ...) from inputs (say *X1*, *X2*, *X3*, ...) identified by
condition port into the *vpz* file. Let us consider, but it is only an option, 
that there is an input (say *Xs*) that represents a random value (eg. the value
of a random generator seed). The functions evaluation returns temporal column of 
outputs. For example *F1* returns a vector *{Y11, Y12,...,Y1n}* where *Y1i* is 
the value of *F1* at time index *i*. 

The MetaManager configuration consists in giving the different values 
*{Xj(1), ..., Xj(p)}* to input *Xj*. The experiment plan size *p* must be the 
same for all *j*. One can also gives the different values of Xs of size *q*.
The MetaManager will perform (using parallel tools) the simulation of the p*q
simulations. The output is a matrix where a line corresponds to an input value
and a column corresponds to a function:

*stat1_k (int1(F1(X1(1), X2(1), X3(1), Xs(k))), stat2_k (int2(F2(X1(1), X2(1), X3(1), Xs(k)))*

...

*stat1_k (int1(F1(X1(p), X2(p), X3(p), Xs(k))), stat2_k (int2(F2(X1(p), X2(p), X3(p), Xs(k)))*

Where *int1* and *int2* gives the function used to integrate 
the temporal vector of outputs of *F1* and *F2* respectively 
(eg. *first*, *last*, ...).
And *stat1_k* and *stat2_k* gives the function for aggregating the *q* 
replicates (eg. *mean*, *max*, ...) of *F1* and *F2* respectively. 

### The MetaManager API.

The MetaManager is configured by a *vle::value::Map* provided by the user.
It can contains:

* **vpz** (string) : it gives the name of the *vpz* file. It allows to
 identify together with *package* the nested model.
* **package** (string) : it gives the name of the *package* where is located
 the nested model. It allows to identify together with *vpz*
 the nested model.
* **input_X** (vle::value::Value) : where X is of the form *condname/portname*,
 it gives for one input the set of value to simulate.
 If a *vle::value::Set* or *vle:value::Tuple* is given, it is understood as
 multiple simulation inputs.
* **output_Y** (vle::value::Map) : where Y is an id for the output. 
 The map should/could provide:
    * **path** (string): is of the form *viewname/pathOfTheAtomicModel.ObsPort*.
  It identifies the column of outputs *ObsPort* computed by the atomic
  model *pathOfTheAtomicModel* and saved into the view *viewname*. 
    * **integration** (string amongst "last", "max", "mse" or "all", default "last"):
  the type of temporal integration to perform.
  the output with id *X*. The string has the following form:
    * **mse_observations** (vle::value::Tuple): required only if integration="mse".
  It gives the series of observations for computing the MSE
    * **mse_times** (vle::value::Tuple): required only if integration="mse". It
  gives times at which the mse_observations are given (mse_observatiosn and
  mse_times must have the same length.
    * **aggregation** (string amongst "mean", "quantile" default "mean"): It gives
  the type of aggregation to perform on the simulations replicates.
* **replicate_X** (vle::value::Value, optional) where X is of the 
 form *condname/portname*, it gives the values for the replicates (eg. seeds).
 It has to be a *vle::value::Set* or *vle:value::Tuple*.
* **config_parallel_type** (string amongst *threads*, *mvle* and *single*;
 default *single*). It sets the type of parallelization to perform.
* **config_parallel_rm_files** (bool; default *true*). Used only 
 if *config_parallel_type* is set to *mvle*. Simulation files created into
 directory *working_dir* are removed after analysis.
* **config_parallel_nb_slots** (int > 0). it gives the number of slots to
 use for parallelization.
* **working_dir** (string). Required only if *config_parallel_type* is set 
 to *mvle*. It gives the working directory where are produced result file of single
 simulations.
 

To use the MetaManager API:

```
#include <vle/recursive/MetaManager.hpp>
...
vle::value::Map init;
...   //fill the init map according the documentation above
vle::recursive::MetaManager meta;
meta.init(init);
const vv::Value& res = meta.launchSimulations();
std::cout << res << std::endl;
```                          

### The MetaManager dynamic

 The MetaManager atomic model has the following dynamic, implemented in PDEVS
 framework:
 * If the conditions provide an initialization map for the MetaManager API,
 it performs the simulation of the experimental plan during the PDEVS 
 initialization step, then it falls asleep for a given duration, then it outputs 
 the results, and then falls asleep indefinitely.
 * Each time it receives an experiment plan setting on its input port, 
 it performs the simulation of the experiment plan, then it falls asleep for a
 duration, then it outputs the results and then falls asleep indefinitely. 
 the results.


Configuring the dynamic consists in giving the elements required for the API 
 configuration directly into conditions ports. Other configuration keys are:
 * **step_duration** (double, default 0). It gives the duration time, for 
 the embedding simulator engine clock, required for the simulation of the
 experiment plan. 
 
### Memory allocation performance tests

First results show that we can expect 10 times less allocated memory using
the MetaManager Dynamic embedded into a vpz. A possible explanation
is the replication of string allocation with the Rvle API, that is useless
in multi simulation.



Script R based on the MetaManager Dynamic:

```
set.seed(12369)
n = 500;
x1 = runif(n,0.0,2.0);
x2 = runif(n,0.0,2.0);

library(rvle)
f = new("Rvle", pkg="vle.recursive", file ="vle-recursive.vpz")
f = setDefault(f,
  duration = 0,
  cond.id_package="vle.recursive_test",
  cond.id_vpz="ExBohachevsky.vpz",
  cond.id_input_x1="cond/x1",
  cond.id_input_x2="cond/x2",
  cond.id_output_y="max[view/ExBohachevsky:ExBohachevsky.y]",#TODO last and not max
  cond.id_output_y_noise="max[view/ExBohachevsky:ExBohachevsky.y_noise]",
  cond.id_replica_xs="",
  cond.output_stat_y="mean",
  cond.output_stat_y_noise="mean",
  cond.values_x1.as_single=x1,
  cond.values_x2.as_single=x2
)
system.time(fres <- results(run(f)))
## user time=0.291, CPU time=0.471, real elapsed time=0.790
print(object.size(fres))
## 57336 bytes
```

Script R directly based on the Rvle API for multi simulation:

```
set.seed(12369)
n = 500;
x1 = runif(n,0.0,2.0);
x2 = runif(n,0.0,2.0);

library(rvle)
g = new("Rvle", pkg="vle.recursive_test", file ="ExBohachevsky.vpz")
g = setDefault(g,
 outputplugin=c(view="vle.output/storage"),
 plan = "linear",
 cond.x1 = x1,
 cond.x2 = x2
)
system.time(gres <- results(run(g)))
## user time=0.338, CPU time=0.483, real elapsed time=0.819
print(object.size(gres))
## 648200 bytes
```

 
 
 
 
 