// Compile-Time Validated Petri Net
// validation architecture - modeling templates

#pragma once

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <tuple>
#include <queue>
#include <set>
#include <sstream>
#include <algorithm>

#define CTVPN_MAJOR_VER 1
#define CTVPN_MINOR_VER 0

namespace ctvpn {

template <size_t PlaceId, size_t Tokens>
struct Arc {
    static constexpr size_t place_id = PlaceId;
    static constexpr size_t weight = Tokens;
};

// Wrapper structs to safely handle multiple input/output parameter packs
template <typename... Arcs>
struct In {};

template <typename... Arcs>
struct Out {};

// Primary template declaration taking two wrapper types
template <typename InputWrapper, typename OutputWrapper>
struct Transition;

// Specialization that unpacks inputs and outputs
template <typename... InputArcs, typename... OutputArcs>
struct Transition<In<InputArcs...>, Out<OutputArcs...>> {
    using inputs = std::tuple<InputArcs...>;
    using outputs = std::tuple<OutputArcs...>;
};

template <size_t N>
constexpr bool IsFireableConstexpr(const std::array<size_t, N>& current, const std::array<size_t, N>& req) {
    for (size_t i = 0; i < N; ++i) {
        if (current[i] < req[i]) return false;
    }
    return true;
}

template <size_t N>
constexpr std::array<size_t, N> FireConstexpr(std::array<size_t, N> current, const std::array<size_t, N>& req, const std::array<size_t, N>& prod) {
    for (size_t i = 0; i < N; ++i) {
        current[i] = current[i] - req[i] + prod[i];
    }
    return current;
}

template <size_t NumPlaces, size_t MaxBound, typename... Transitions>
class PetriNetPipeline {
private:
    using Marking = std::array<size_t, NumPlaces>;

    template <typename ArcsTuple>
    static constexpr Marking make_static_vector() {
        Marking vec{};
        auto extract = [&vec](auto arc) { vec[decltype(arc)::place_id] = decltype(arc)::weight; };
        std::apply([&](auto... args) { (extract(args), ...); }, ArcsTuple{});
        return vec;
    }

    static std::string to_dot_id(const std::vector<size_t>& m) {
        std::stringstream ss; 
        ss << "M_";
        for (size_t i = 0; i < m.size(); ++i) { 
            ss << m[i] << (i < m.size() - 1 ? "_" : ""); 
        }
        return ss.str();
    }

    static std::string to_string(const std::vector<size_t>& m) {
        std::stringstream ss; 
        ss << "[";
        for (size_t i = 0; i < m.size(); ++i) { 
            ss << m[i] << (i < m.size() - 1 ? ", " : ""); 
        }
        ss << "]";
        return ss.str();
    }

public:
    // Verify static invariants with an optional expected final (terminal) marking
    static constexpr bool VerifyStatic(Marking initial, Marking expected_final = {}) {
        std::vector<Marking> visited;
        std::vector<Marking> queue;
        size_t queue_start = 0;

        Marking req_vectors[sizeof...(Transitions)] = { make_static_vector<typename Transitions::inputs>()... };
        Marking prod_vectors[sizeof...(Transitions)] = { make_static_vector<typename Transitions::outputs>()... };

        queue.push_back(initial);

        while (queue_start < queue.size()) {
            Marking current = queue[queue_start++];
            
            if (std::find(visited.begin(), visited.end(), current) != visited.end()) {
                continue;
            }
            visited.push_back(current);

            // Boundedness checking
            for (size_t i = 0; i < NumPlaces; ++i) {
                if (current[i] > MaxBound) return false; 
            }

            bool enabled = false;
            for (size_t t = 0; t < sizeof...(Transitions); ++t) {
                if (IsFireableConstexpr<NumPlaces>(current, req_vectors[t])) {
                    enabled = true;
                    auto next = FireConstexpr<NumPlaces>(current, req_vectors[t], prod_vectors[t]);
                    
                    if (std::find(visited.begin(), visited.end(), next) == visited.end() &&
                        std::find(queue.begin() + queue_start, queue.end(), next) == queue.end()) {
                        queue.push_back(next);
                    }
                }
            }

            // Liveness checking: if no transitions are enabled, check if it's the valid terminal state
            if (!enabled) {
                if (current != expected_final) {
                    return false; // Unexpected deadlock!
                }
            }
        }
        return true;
    }

    static void GenerateDotGraph(
        const std::array<size_t, NumPlaces>& initial,
        const std::string strFDot = "reachability_graph.dot"
    ) {

        std::vector<Marking> req_vectors = { make_static_vector<typename Transitions::inputs>()... };
        std::vector<Marking> prod_vectors = { make_static_vector<typename Transitions::outputs>()... };

        std::vector<size_t> init_vec(initial.begin(), initial.end());
        std::set<std::vector<size_t>> visited;
        std::queue<std::vector<size_t>> q;

        std::ofstream dot(strFDot.c_str());
        dot << "digraph ReachabilityGraph {\n    rankdir=TB;\n"
            << "    node [shape=box, style=\"rounded,filled\", fillcolor=\"#E3F2FD\", fontname=\"Courier\"];\n"
            << "    edge [fontname=\"Arial\", fontsize=10];\n\n";
        
        q.push(init_vec);
        visited.insert(init_vec);

        dot << "    " << to_dot_id(init_vec) << " [label=\"" << to_string(init_vec) << "\", fillcolor=\"#C8E6C9\", penwidth=2];\n";
        
        while (!q.empty()) {
            auto current = q.front(); 
            q.pop();
            bool has_moves = false;
            
            for (size_t i = 0; i < sizeof...(Transitions); ++i) {
                bool fireable = true;
                for (size_t p = 0; p < NumPlaces; ++p) {
                    if (current[p] < req_vectors[i][p]) fireable = false;
                }
                
                if (fireable) {
                    has_moves = true;
                    std::vector<size_t> next = current;
                    for (size_t p = 0; p < NumPlaces; ++p) {
                        next[p] = next[p] - req_vectors[i][p] + prod_vectors[i][p];
                    }
                    
                    dot << "    " << to_dot_id(current) << " -> " << to_dot_id(next) << " [label=\" T" << i << " \"];\n";
                    
                    if (visited.find(next) == visited.end()) {
                        visited.insert(next); 
                        q.push(next);
                        dot << "    " << to_dot_id(next) << " [label=\"" << to_string(next) << "\"];\n";
                    }
                }
            }

            if (!has_moves) {
                dot << "    " << to_dot_id(current) << " [fillcolor=\"#FFCDD2\", color=\"#B71C1C\"];\n";
            }
        }

        dot << "}\n";
        dot.close();

        std::cout << "[Runtime Success] Reachability matrix compiled into 'reachability_graph.dot'\n";
    }
};

} // namespace ctvpn
