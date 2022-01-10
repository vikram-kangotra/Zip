#pragma once

#include <vector>
#include <tuple>
#include <functional>

template <typename T>
concept ZipIterable =
    requires(T t) {
        typename T::value_type; 
        typename T::iterator;
        t.begin();
        t.end();
        t.size();
    };

template <ZipIterable... Ts>
class zip {
    private:
        
        using valueTuple = std::tuple<typename Ts::value_type&...>;
        using iteratorContainer = std::tuple<typename Ts::iterator...>;

        template <unsigned... S> struct seq { using type = seq<S...>; };
        template <unsigned max, unsigned... S> struct make_seq : make_seq<max-1, max-1, S...> {};
        template <unsigned... S> struct make_seq<0, S...> : seq<S...> {};

        template <unsigned... S>
        void advance_iterators(seq<S...>, iteratorContainer& it) {
            (++std::get<S>(it), ...);
        }

        template <typename... Rs>
        void advance_iterators(std::tuple<Rs...>& it) {
            advance_iterators( make_seq<sizeof...(Rs)>(), it);
        }

        template <unsigned... S>
        auto make_tuple_from(seq<S...>, iteratorContainer& it) {
            return std::make_tuple(std::ref(*std::get<S>(it))...);
        }
        
        template <typename... Rs>
        auto make_tuple_from(std::tuple<Rs...>& it) {
            return make_tuple_from( make_seq<sizeof...(Rs)>(), it );
        }

    public:

        zip(Ts&... containers) {
            iteratorContainer iterators = { containers.begin()... };
            const size_t size = minSize( containers... );
            for (size_t i = 0; i < size; ++i) {
                zip_containers.emplace_back( make_tuple_from(iterators) ); 
                advance_iterators( iterators );
            }
        }

        size_t minSize(Ts&... containers) {
            std::vector vec = { containers.size()... };
            std::sort(vec.begin(), vec.end());
            return vec[0];
        }

        auto begin() {
            return zip_containers.begin();
        }

        auto end() {
            return zip_containers.end();
        }

    private:
        
        std::vector<valueTuple> zip_containers;
};
