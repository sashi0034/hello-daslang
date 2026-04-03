#include "daScript/daScript.h"
#include "daScript/ast/ast_interop.h"
#include "daScript/ast/ast_typefactory_bind.h"
#include "double3_bindings.h"

#include <iostream>

MAKE_TYPE_FACTORY(Double2, Double2)
MAKE_TYPE_FACTORY(Double3, Double3)

Double2::Double2() : x(0.0), y(0.0) {
}

Double2::Double2(double xValue, double yValue) : x(xValue), y(yValue) {
}

Double2 Double2::add(const Double2& rhs) const {
    return Double2{x + rhs.x, y + rhs.y};
}

Double2 Double2::scale(double factor) const {
    return Double2{x * factor, y * factor};
}

void Double2::print() const {
    std::cout << "[cpp] Double2(x=" << x << ", y=" << y << ")\n";
}

Double3::Double3() : x(0.0), y(0.0), z(0.0) {
}

Double3::Double3(double xValue, double yValue, double zValue)
    : x(xValue), y(yValue), z(zValue) {
}

Double3 Double3::add(const Double3& rhs) const {
    return Double3{x + rhs.x, y + rhs.y, z + rhs.z};
}

Double3 Double3::scale(double factor) const {
    return Double3{x * factor, y * factor, z * factor};
}

Double3 Double3::transform() const {
    return Double3{z + 10.0, x + 20.0, y + 30.0};
}

void Double3::print() const {
    std::cout << "[cpp] Double3(x=" << x << ", y=" << y << ", z=" << z << ")\n";
}

Double3 Double2::toDouble3(double zValue) const {
    return Double3{x, y, zValue};
}

void print_double3_from_cpp(const Double3& v) {
    v.print();
}

namespace das {

using double2_add_method = DAS_CALL_MEMBER(Double2::add);
using double2_scale_method = DAS_CALL_MEMBER(Double2::scale);
using double2_to_double3_method = DAS_CALL_MEMBER(Double2::toDouble3);
using double2_print_method = DAS_CALL_MEMBER(Double2::print);
using double3_add_method = DAS_CALL_MEMBER(Double3::add);
using double3_scale_method = DAS_CALL_MEMBER(Double3::scale);
using double3_transform_method = DAS_CALL_MEMBER(Double3::transform);
using double3_print_method = DAS_CALL_MEMBER(Double3::print);

struct Double2Annotation final : ManagedStructureAnnotation<Double2, false> {
    explicit Double2Annotation(ModuleLibrary& lib) : ManagedStructureAnnotation("Double2", lib) {
        addField<DAS_BIND_MANAGED_FIELD(x)>("x", "x");
        addField<DAS_BIND_MANAGED_FIELD(y)>("y", "y");
    }

    bool isPod() const override {
        return true;
    }

    bool isRawPod() const override {
        return false;
    }

    bool hasNonTrivialCtor() const override {
        return false;
    }
};

struct Double3Annotation final : ManagedStructureAnnotation<Double3, false> {
    explicit Double3Annotation(ModuleLibrary& lib) : ManagedStructureAnnotation("Double3", lib) {
        addField<DAS_BIND_MANAGED_FIELD(x)>("x", "x");
        addField<DAS_BIND_MANAGED_FIELD(y)>("y", "y");
        addField<DAS_BIND_MANAGED_FIELD(z)>("z", "z");
    }

    bool isPod() const override {
        return true;
    }

    bool isRawPod() const override {
        return false;
    }

    bool hasNonTrivialCtor() const override {
        return false;
    }
};

class Module_Double3CppDemo final : public Module {
public:
    Module_Double3CppDemo() : Module("double3cpp_bindings") {
        ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addAnnotation(make_smart<Double2Annotation>(lib));
        addAnnotation(make_smart<Double3Annotation>(lib));

        addCtorAndUsing<Double2>(*this, lib, "Double2", "Double2");
        addCtorAndUsing<Double2, double, double>(*this, lib, "Double2", "Double2");
        addCtorAndUsing<Double3>(*this, lib, "Double3", "Double3");
        addCtorAndUsing<Double3, double, double, double>(*this, lib, "Double3", "Double3");

        addExtern<DAS_CALL_METHOD(double2_add_method), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "add",
            SideEffects::none,
            DAS_CALL_MEMBER_CPP(Double2::add)
        )
            ->args({"self", "rhs"});

        addExtern<DAS_CALL_METHOD(double2_scale_method), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "scale",
            SideEffects::none,
            DAS_CALL_MEMBER_CPP(Double2::scale)
        )
            ->args({"self", "factor"});

        addExtern<DAS_CALL_METHOD(double2_to_double3_method), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "toDouble3",
            SideEffects::none,
            DAS_CALL_MEMBER_CPP(Double2::toDouble3)
        )
            ->args({"self", "zValue"});

        addExtern<DAS_CALL_METHOD(double2_print_method)>(
            *this,
            lib,
            "print",
            SideEffects::modifyExternal,
            DAS_CALL_MEMBER_CPP(Double2::print)
        )
            ->args({"self"});

        addExtern<DAS_CALL_METHOD(double3_add_method), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "add",
            SideEffects::none,
            DAS_CALL_MEMBER_CPP(Double3::add)
        )
            ->args({"self", "rhs"});

        addExtern<DAS_CALL_METHOD(double3_scale_method), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "scale",
            SideEffects::none,
            DAS_CALL_MEMBER_CPP(Double3::scale)
        )
            ->args({"self", "factor"});

        addExtern<DAS_CALL_METHOD(double3_transform_method), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "transform",
            SideEffects::none,
            DAS_CALL_MEMBER_CPP(Double3::transform)
        )
            ->args({"self"});

        addExtern<DAS_CALL_METHOD(double3_print_method)>(
            *this,
            lib,
            "print",
            SideEffects::modifyExternal,
            DAS_CALL_MEMBER_CPP(Double3::print)
        )
            ->args({"self"});

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

REGISTER_MODULE_IN_NAMESPACE(Module_Double3CppDemo, das)
