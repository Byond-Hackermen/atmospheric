# Asyncmos
Poorly named attempt at reverse engineering BYOND's server software. This project aims to enable, among others, variable modification and proc calling outside of DM code. It is written in C++, compiled into a DLL and loaded via `call()()`.

#### How do I check if it works?
Download the repository and [premake5](https://premake.github.io/download.html). Go into the `asyncmos` directory and run `premake5 [environment]` (For example, if you use Visual Studio 2017, run `premake5 vs2017`). This creates necessary files in `asyncmos/asyncmos`. Open the solution file and hit `Build Solution`. Compilation should finish successfully. Don't pay attention to the warnings.
Afterwards, go to `sample_dm_project`, open the .dme file, and compile it. The only thing left is obtain `EasyHook32.dll` and place it next to the .dmb. You can ask us for a copy until we figure out how to get rid of it.
Open Dream Daemon, start the dmb, join up. Click `init dll` in the verb list. If Dream Daemon didn't show any errors, click `test dll`. You should get 5 messages like this: `X test successful! Argument: Y`. If the tests succeeded and DD didn't crash, hooray! The project is functional.

#### The API
##### BYOND::Object
Represents all kinds of data, including numbers, strings, objects... You may get the type by calling `Type()`, which returns a `BYOND::VariableType`. To convert it to something saner, you may use one of the following:
* `AsNumber()` - Returns a float.
* `AsString()` - Returns an std::string.
* `AsList()` - Returns a `BYOND::List`.
* `As(BYOND::DatumObject type)` - Returns a subtype of BYOND::DatumObject.

#### BYOND::List
A list of `BYOND::Object`s. Normal lists support item getting and setting, associative lists only support getting.
* `Length()` - Returns the length of the list.
* `At(unsigned int index)` - Returns the `BYOND::Object` at index `i`. Invalid indexes return a `nullptr`.
* `list[unsigned int index]` - Equivalent to `At(index)`.
* `list[BYOND::Object index]` - Behaves like `At()` but for associative lists.

#### BYOND::DatumObject
Represents an atom or datum. You can get and set variables and call procs using this.
* `Set(std::string varName, VariableType varType, unsigned int new_value)`
* `Set(std::string varName, std::string new_value)`
* `Set(std::string varName, VariableType varType, float new_value)`
* `BYOND::Object GetVariable(std::string varName)`
* `T Get<T>(std::string varName)` - You may pass a subtype of `DatumObject`, an `std::string`, `float`, or `BYOND::List` as T.
* `BYOND::Object Call(std::string procName, std::vector<Object> arguments)` - For convenience you may pass the arguments as an initializer list: `x.Call("proc", { BYOND::Object("Hello, world!") })`
##### vars
The global variable `BYOND::Variables vars` is used to interact with global stuff.
* `BYOND::Object ReadGlobalVariable(std::string name);`
* `BYOND::Object CallGlobalProc(std::string procName);`
* `BYOND::Object CallGlobalProc(std::string procName, std::vector<BYOND::Object> arguments);`

## Example
	float dirs[] = { 1, 2, 4, 8 };
	while (true) {
		BYOND::List lst = vars.ReadGlobalVariable("mob_list").AsList();
		for (int i = 0; i < lst.Length(); i++)
		{
			BYOND::Mob mob = lst.At(i)->As(BYOND::Mob);
			mob.Set("dir", BYOND::VariableType::Number, dirs[rand()%4]);
			Sleep(1);
		}
	}