# Date & Time Gem
This extension (*gem*) for [O3DE (Open 3D Engine)](https://www.o3de.org) provides a set of classes and components to work with time. Its modular architecture allows to instance at runtime only what you need:

### Time
- A class to manage time in a human-readable format, such as `Hours`, `Minutes` and `Seconds`. It supports basic arithmetic operations on individual fields (e.g. `AddMinutes`, `SubtractHours`) or on its entirety (e.g. `FromSeconds` with a standard timestamp).
- A runtime component to count time using an event-based approach. Other components in the scene can listen for time changes from it, filtering only on the required fields.
- Different options to separate the simulation time from the real one, converting values in both directions.

### Dates
- A class to manage dates in a human-readable format, such as `Year`, `Month` and `Day`. It supports basic arithmetic operations on individual fields (e.g. `AddMonths`, `SubtractYears`) or on its entirety (e.g. `FromSeconds` with a standard timestamp).
- A runtime component to count dates using an event-based approach. Other components in the scene can listen for date changes from it, filtering on the required fields only.
- Designed to support different calendars (only `Gregorian` is implemented at the moment).

### Alarms
- A runtime component to set alarms at specific simulation instants, in order to execute code callbacks or to notify a list of entities in the scene.
- Multiple helper classes to define how an alarm can be repeated multiple times, setting its repeating interval (e.g. `Every <X> Minutes`, `Every <X> Months`) or its end condition (e.g. `For <N> Times`, `Not After <Y>`). Date rules can be combined with time rules to achieve complex schedules of actions.
- A convenient widget with constrained fields to assist setting alarms directly from the editor interface, without the need to write any line of code.

*VIDEO*

## Install
Please choose one of the following options to install this gem in your O3DE projects:

### Automatic installation
Following steps require to install **O3Tanks**, a version manager for O3DE to streamline the development workflow. You can find additional information at its [main repository](https://github.com/loherangrin/o3tanks/).
```
o3tanks install gem https://<this_repository_url>.git --as <any_name>
o3tanks add gem <any_name> --project <project_path>
```

### Manual installation
1. Clone this repository into a directory of your choice:
```
git clone https://<this_repository_url>.git <gem_dir>
```
2. Register the gem to your engine, following the steps that are described in [Registering Gems to a Project](https://www.o3de.org/docs/user-guide/project-config/register-gems/) page of the official O3DE documentation.
3. Activate the gem in your project, following the steps that are described in [Adding and Removing Gems in a Project](https://www.o3de.org/docs/user-guide/project-config/add-remove-gems/). You will have to look for *Date & Time* as gem name in the Catalog list.

## Examples
A sample project showing the basic usage of this gem is located under `/examples/` directory. Three different variants are available:
- `examples/cpp`: implementation using C++ (native code).
- `examples/lua`: implementation using Lua (scripting).
- `examples/script-canvas`: implementation using ScriptCanvas (visual scripting). 

You can inspect each project variant adding it to your O3DE Project Manager (at least version `2111.2` is required). Or, if you are using **O3Tanks**, you can even run it directly as a standalone application:
```
cd <gem_dir>/examples/<implementation>
o3tanks build client
o3tanks run client
```

## Contribute
We are happy to receive your feeback! Please open an issue in [our tracker](https://github.com/loherangrin/addons.o3de.date-time/issues) if you would like to propose a new feature or you have encountered any error.

To get involved in the development of o3tanks, please refer to our [contribution guidelines](./CONTRIBUTING.md).

## License
**Date & Time Gem** is licensed under the Apache License, version 2.0. Please see [LICENSE](./LICENSE) and [NOTICE](./NOTICE) files for further details.
