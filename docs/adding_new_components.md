# Adding New Components

- First create the component struct in `src/engine/components` under `Engine` namespace
- To make the component serializable:
	- Use the cereal serialize function in `src/engine/serialize/cereal_serialize_custom_types.h`
	- Modify `Engine::snapshot_get_func_engine_types` from `src/engine/serialize/entt_registry_serializer_get_func.h` and add this line 
	```cpp
	snapshot.template get<Engine::YouComponentType>(archive);
	```
- To make the component usable in Editor:
	- Use `ImReflect` macros/`tag_invoke` to reflect your type in `src/reflect/imreflect_custom_types.h`
	- Modify `Reflect::register_all_components` from `src/reflect/reflect.cpp` and add this line
	```cpp
	register_component<Engine::YourComponentType>("YourComponentName");
	```
- To make the component usable from lua scripts:
	- Register sol usertype in `Reflect::register_sol_usertypes` from `src/reflect/reflect_sol.cpp`