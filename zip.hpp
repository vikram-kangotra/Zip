#pragma once

#include <vector>
#include <tuple>

template <typename T>
concept Container = 
    requires(T t) {
        typename T::value_type;
        typename T::iterator;
        t.begin();
        t.end();
        t.size();
    };

template <Container... Ts>
class zip {
    private:
    	using tuple_type = std::tuple<typename Ts::value_type...>;
        using iterator_container = std::tuple<typename Ts::iterator...>;

        template<unsigned...s> struct seq { typedef seq<s...> type; };
        template<unsigned max, unsigned... s> struct make_seq:make_seq<max-1, max-1, s...> {};
        template<unsigned...s> struct make_seq<0, s...>:seq<s...> {};
        
        template <unsigned... S>
        void advance_iterator(seq<S...>, iterator_container& it) {
            (++std::get<S>(it), ...);
        }

        template <typename... Rs>
        auto advance_iterator(std::tuple<Rs...>& it) {
            advance_iterator(make_seq<sizeof...(Rs)>(), it);
        }

        template <unsigned... S>
        auto make_tuple_from(seq<S...>, iterator_container& it) {
            return std::make_tuple(*std::get<S>(it)...);
        }

        template<typename... Rs>
        auto make_tuple_from(std::tuple<Rs...>& it) {
          return make_tuple_from(make_seq<sizeof...(Rs)>(), it);
        }

    public:
        constexpr zip(Ts&... containers) {
        	const auto size = minSize(containers...);
            iterator_container iterators = { containers.begin()... };
            for ( size_t i = 0; i < size; ++i ) {
            	zip_container.emplace_back( make_tuple_from(iterators) );
                advance_iterator(iterators);
            }
        }
         
        size_t minSize(Ts&... container) {
        	std::vector arr = { container.size()... };
            std::sort(arr.begin(), arr.end());
            return arr[0];
        }
        
        auto begin() {
        	return zip_container.begin();
        }
        
        auto end() {
        	return zip_container.end();
        }

    private:
        std::vector<tuple_type> zip_container;
};
