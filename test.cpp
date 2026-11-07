#include <iostream>
#include <cassert>
#include <array>
#include "ctvpn.h"
#include "generated_net.hpp"

// places
constexpr size_t P_Idle = 0;
constexpr size_t P_Active = 1;
constexpr size_t P_Finished = 2;
constexpr size_t P_Resource = 3;

using T_Start = ctvpn::Transition<ctvpn::In<ctvpn::Arc<P_Idle, 1>>, ctvpn::Out<ctvpn::Arc<P_Active, 1>>>;
using T_End = ctvpn::Transition<ctvpn::In<ctvpn::Arc<P_Active, 1>>, ctvpn::Out<ctvpn::Arc<P_Finished, 1>>>;

using T_ComplexStart = ctvpn::Transition<
    ctvpn::In<ctvpn::Arc<P_Idle, 1>, ctvpn::Arc<P_Resource, 1>>, 
    ctvpn::Out<ctvpn::Arc<P_Active, 1>>
>;

void test_successful_pipeline() {

    std::cout << "[Test 1] Running standard live & bounded pipeline test...\n";

    using Pipeline = ctvpn::PetriNetPipeline<3, 2, T_Start, T_End>;

    // init-marking: {1, 0, 0}, terminal-marking: {0, 0, 1}
    constexpr bool is_valid = Pipeline::VerifyStatic({1, 0, 0}, {0, 0, 1});
    assert(is_valid == true);

    std::cout << "  -> Test 1 Passed!\n";
}

void test_deadlock_detection() {

    std::cout << "[Test 2] Running deadlock detection test...\n";

    // only T_Start -> to {0, 1, 0} with no outgoing moves
    using PipelineSingle = ctvpn::PetriNetPipeline<3, 2, T_Start>;

    constexpr bool is_valid = PipelineSingle::VerifyStatic({1, 0, 0});
    assert(is_valid == false); // should catch deadlock

    std::cout << "  -> Test 2 Passed!\n";
}

void test_bound_violation() {

    std::cout << "[Test 3] Running token boundedness violation test...\n";

    // transition that produces 3 tokens in P_Active
    using T_Overflow = ctvpn::Transition<
        ctvpn::In<ctvpn::Arc<P_Idle, 1>>, 
        ctvpn::Out<ctvpn::Arc<P_Active, 3>>
    >;

    // max bound allowed is set to 2, but T_Overflow produces 3
    using PipelineOverflow = ctvpn::PetriNetPipeline<3, 2, T_Overflow>;

    constexpr bool is_valid = PipelineOverflow::VerifyStatic({1, 0, 0});
    assert(is_valid == false); // Should catch bound violation and return false

    std::cout << "  -> Test 3 Passed!\n";
}

void test_multi_arc_transition() {

    std::cout << "[Test 4] Running multi-arc transition test...\n";

    // 4 places: Idle (0), Active (1), Finished (2), Resource (3)
    using PipelineMulti = ctvpn::PetriNetPipeline<4, 2, T_ComplexStart, T_End>;

    // initial-marking: {1, 0, 0, 1} (Idle=1, Resource=1), final-marking: {0, 0, 1, 0} (Finished=1)
    constexpr bool is_valid = PipelineMulti::VerifyStatic({1, 0, 0, 1}, {0, 0, 1, 0});
    assert(is_valid == true);

    std::cout << "  -> Test 4 Passed!\n";
}

void test_dot_graph_generation() {

    std::cout << "[Test 5] Running DOT graph generation test...\n";

    using Pipeline = ctvpn::PetriNetPipeline<3, 2, T_Start, T_End>;
    std::array<size_t, 3> initial_marking = {1, 0, 0};

    // generates 'reachability_graph.dot' at runtime
    Pipeline::GenerateDotGraph(initial_marking);

    std::cout << "  -> Test 5 Passed! ('reachability_graph.dot' created)\n";
}

void test_dot_generated_model() {

    std::cout << "[Test 6] Running DOT graph for model.pnml test...\n";

    using Pipeline = ctvpn::PetriNetPipeline<2, 1, generated_net::T_T0>;
    std::array<size_t, 2> initial_marking = {1, 0};

    Pipeline::GenerateDotGraph(initial_marking, "model_reachability_graph.dot");

    std::cout << "  -> Test 6 Passed! ('model_reachability_graph.dot' created)\n";
}

int main() {

    std::cout << "   Running Complete CTVPN Unit Tests   \n";

    test_successful_pipeline();
    test_deadlock_detection();
    test_bound_violation();
    test_multi_arc_transition();
    test_dot_graph_generation();
    test_dot_generated_model();

    std::cout << "   All Unit Tests Passed Successfully! \n";

    return 0;
}
