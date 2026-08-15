#ifndef POPULATION_INITIALIZER_HPP
#define POPULATION_INITIALIZER_HPP

template<typename IGetSize, typename IGetCapacity, typename IAddAgent, typename IProduceInitialAgent>
struct population_initializer {
    population_initializer(IGetSize& get_size, IGetCapacity& get_capacity, IAddAgent& add_agent,
                           IProduceInitialAgent& produce_initial_agent);
    void initialize();
private:
    IGetSize& get_size_;
    IGetCapacity& get_capacity_;
    IAddAgent& add_agent_;
    IProduceInitialAgent& produce_initial_agent_;
};

template<typename IGetSize, typename IGetCapacity, typename IAddAgent, typename IProduceInitialAgent>
population_initializer<IGetSize, IGetCapacity, IAddAgent, IProduceInitialAgent>::
    population_initializer(IGetSize& get_size, IGetCapacity& get_capacity, IAddAgent& add_agent,
                           IProduceInitialAgent& produce_initial_agent)
    : get_size_(get_size)
    , get_capacity_(get_capacity)
    , add_agent_(add_agent)
    , produce_initial_agent_(produce_initial_agent) {
}

template<typename IGetSize, typename IGetCapacity, typename IAddAgent, typename IProduceInitialAgent>
void population_initializer<IGetSize, IGetCapacity, IAddAgent, IProduceInitialAgent>::initialize() {
    while(get_size_.size() < get_capacity_.capacity())
        add_agent_.add(produce_initial_agent_.produce());
}

#endif
