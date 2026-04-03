#include "daScript/daScript.h"
#include "daScript/ast/ast_interop.h"
#include "daScript/ast/ast_typefactory_bind.h"
#include "double3_bindings.h"

#include <iostream>

MAKE_TYPE_FACTORY(Double3, Double3)

Double3 make_double3(double x, double y, double z) {
    return Double3{x, y, z};
}

Double3 add_double3(const Double3& lhs, const Double3& rhs) {
    return Double3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Double3 scale_double3(const Double3& v, double s) {
    return Double3{v.x * s, v.y * s, v.z * s};
}

Double3 cpp_transform(const Double3& v) {
    return Double3{v.z + 10.0, v.x + 20.0, v.y + 30.0};
}

void print_double3_from_cpp(const Double3& v) {
    std::cout << "[cpp] Double3(x=" << v.x << ", y=" << v.y << ", z=" << v.z << ")\n";
}

namespace das {

struct Double3Annotation final : ManagedStructureAnnotation<Double3, false> {
    explicit Double3Annotation(ModuleLibrary& lib) : ManagedStructureAnnotation("Double3", lib) {
        addField<DAS_BIND_MANAGED_FIELD(x)>("x", "x");
        addField<DAS_BIND_MANAGED_FIELD(y)>("y", "y");
        addField<DAS_BIND_MANAGED_FIELD(z)>("z", "z");
    }
};

class Module_Double3Demo final : public Module {
public:
    Module_Double3Demo() : Module("double3_bindings") {
        ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addAnnotation(make_smart<Double3Annotation>(lib));

        addExtern<DAS_BIND_FUN(make_double3), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "make_double3",
            SideEffects::none,
            "make_double3"
        )
            ->args({"x", "y", "z"});

        addExtern<DAS_BIND_FUN(add_double3), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "add_double3",
            SideEffects::none,
            "add_double3"
        )
            ->args({"lhs", "rhs"});

        addExtern<DAS_BIND_FUN(scale_double3), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "scale_double3",
            SideEffects::none,
            "scale_double3"
        )
            ->args({"v", "s"});

        addExtern<DAS_BIND_FUN(cpp_transform), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "cpp_transform",
            SideEffects::none,
            "cpp_transform"
        )
            ->args({"v"});

        addExtern<DAS_BIND_FUN(print_double3_from_cpp)>(
            *this,
            lib,
            "print_double3_from_cpp",
            SideEffects::modifyExternal,
            "print_double3_from_cpp"
        )
            ->args({"v"});

        verifyAotReady();
    }

    ModuleAotType aotRequire(TextWriter& tw) const override {
        tw << "#include \"double3_bindings.h\"\n";
        return ModuleAotType::cpp;
    }
};

}  // namespace das

REGISTER_MODULE_IN_NAMESPACE(Module_Double3Demo, das)
