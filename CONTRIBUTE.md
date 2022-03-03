Contributing Guide
====

Code formatting
----

Formatting is done via clang-format with a wrapper script (`format.sh`) to be able to indent preprocessor directives in the middle of code.
If the formatter is misbehaving for a specific file or section of code feel free to turn it off using `clang-format <on/off>` but address the reason for turning it off in a comment.

Controllers uses the following naming convention:
- Variables (`snake_case`): `variable_name`
- C pointer (`snake_case`) with prefix `p_`: `p_pointer_name`
- C pointer of pointer (`snake_case`) with prefix `pp_`: `pp_pointer_of_pointer_name`
- Functions prefix `Ctrl_` followed by module name (ex. Cuda, OpenCLGpu) and function name both `PascalCase`:  `Ctrl_ModuleName_FunctionName`
- Enum and struct types same as functions: `Ctrl_ModuleName_TypeName`
- Constants `_CTRL_` prefix followed by constant name in `UPPER_SNAKE_CASE` and underscore suffix `_` (in case is a header constant add `H_` suffix): `_CTRL_CONSTANT_NAME_[H_]`

Add new controller
----

Create a new folder with the new controller name at paths:
- `Ctrl/inc/Architectures` Headers for the controller type.
- `Ctrl/inc/Kernel/Architectures` Headers for kernels prototyping, wrapper and launch.
- `Ctrl/src/Architectures` Source code for the controller.

Inside `Ctrl/inc/Architectures` you should write headers defining the structure of your controller. These headers files can also define the structure with additional information about the tile. Your controller should implement the following function prototypes:
- `void Ctrl_<ControllerName>_Create(Ctrl_<ControllerName> *p_ctrl, Ctrl_Policy policy, <extra args if needed>);` Create controller context with internal variables.
- `void Ctrl_<ControllerName>_Destroy(Ctrl_<ControllerName> *p_ctrl);` Destroy internal variables related to controller context.
- `void Ctrl_<ControllerName>_Sync(Ctrl_<ControllerName> *p_ctrl);` Sync controller with main thread if queues are enabled. Does nothing otherwise.
- `void Ctrl_<ControllerName>_CreateTile(Ctrl_<ControllerName> *p_ctrl, Ctrl_Task *p_task);` Allocate memory for a new tile in the controller.
- `void Ctrl_<ControllerName>_InitTile(Ctrl_<ControllerName> *p_ctrl, Ctrl_Task *p_task);` Initialize controller tile.
- `void Ctrl_<ControllerName>_EvalTask(Ctrl_<ControllerName> *p_ctrl, Ctrl_Task *p_task);` Evaluate the following types of tasks (`Ctrl_Task` member `type`):
	- `CTRL_TASK_TYPE_KERNEL`
	- `CTRL_TASK_TYPE_HOST`
	- `CTRL_TASK_TYPE_GLOBALSYNC`
	- `CTRL_TASK_TYPE_ALLOCTILE`
	- `CTRL_TASK_TYPE_DOMAINTILE`
	- `CTRL_TASK_TYPE_SELECTTILE`
	- `CTRL_TASK_TYPE_FREETILE`
	- `CTRL_TASK_TYPE_MOVETO`
	- `CTRL_TASK_TYPE_MOVEFROM`
	- `CTRL_TASK_TYPE_WAITTILE`
	- `CTRL_TASK_TYPE_DESTROYCNTRL`
	- `CTRL_TASK_TYPE_SETDEPENDANCEMODE`

Inside `Ctrl/inc/Kernel/Architectures` your header should define the following macros:
- `CTRL_KERNEL_<MODULE_NAME>_DEVICE_DATA(name, type)` Casting from HitTile data pointer to KTile data pointer according to your controller.
- `CTRL_KERNEL_<MODULE_NAME>_KERNEL_CHAR(name, type, dims, ...)` Kernel characterization called from `CTRL_KERNEL_CHAR( name, type, dims, ... )`.
- `CTRL_KERNEL_<MODULE_NAME>(name, type, ...)` or/and `CTRL_KERNEL_FUNCTION_<MODULE_NAME>(name, type, ...)` Definition of specific kernels for your controller, called by `CTRL_KERNEL( name, type, ... )` and CTRL_KERNEL_FUNCTION( name, type, ... ), respectively. The first macro must take the kernel code as its last parameter, whereas the second is used as the kernel prototype and immediately precedes its definition.
- `CTRL_KERNEL_WRAP_<MODULE_NAME>(name, argsList, type, ...)` Wrapper of kernel launch calls via the function defined by the previous macros.
- `CTRL_KERNEL_<MODULE_NAME>_GENERIC(name, type, ...)` Definition of _generic_ kernel called from `CTRL_KERNEL_GENERIC( name, type, ... )`. This macro must take the kernel code as its last parameter.
- `CTRL_KERNEL_WRAP_<MODULE_NAME>_GENERIC(name, args_list, type, n_args, ...)` Wrapper of _generic_ kernel launch calls via the function defined by the previous macro.

__Important__: All headers should be Once-Only headers, they should be encapsulled into an `#ifndef`region driven by header constant. More information [here](https://gcc.gnu.org/onlinedocs/cpp/Once-Only-Headers.html).
```c
/* File foo.  */
#ifndef _CTRL_FILE_FOO_H_
#define _CTRL_FILE_FOO_H_

the entire file

#endif // _CTRL_FILE_FOO_H_
```
