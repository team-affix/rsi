#ifndef SRF_STEPPER_HPP
#define SRF_STEPPER_HPP

template<typename ISelect, typename IReproduce, typename IGerminate, typename IProduceInitialAgent,
         typename IAddToBuffer, typename IAtCapacity, typename ICommitBuffer>
struct srf_stepper {
    srf_stepper(ISelect& select, IReproduce& reproduce, IGerminate& germinate,
                IProduceInitialAgent& produce_initial_agent, IAddToBuffer& add_to_buffer,
                IAtCapacity& at_capacity, ICommitBuffer& commit_buffer);
    void step();
private:
    ISelect& select_;
    IReproduce& reproduce_;
    IGerminate& germinate_;
    IProduceInitialAgent& produce_initial_agent_;
    IAddToBuffer& add_to_buffer_;
    IAtCapacity& at_capacity_;
    ICommitBuffer& commit_buffer_;
};

template<typename ISelect, typename IReproduce, typename IGerminate, typename IProduceInitialAgent,
         typename IAddToBuffer, typename IAtCapacity, typename ICommitBuffer>
srf_stepper<ISelect, IReproduce, IGerminate, IProduceInitialAgent, IAddToBuffer, IAtCapacity,
            ICommitBuffer>::srf_stepper(ISelect& select, IReproduce& reproduce,
                                        IGerminate& germinate,
                                        IProduceInitialAgent& produce_initial_agent,
                                        IAddToBuffer& add_to_buffer, IAtCapacity& at_capacity,
                                        ICommitBuffer& commit_buffer)
    : select_(select)
    , reproduce_(reproduce)
    , germinate_(germinate)
    , produce_initial_agent_(produce_initial_agent)
    , add_to_buffer_(add_to_buffer)
    , at_capacity_(at_capacity)
    , commit_buffer_(commit_buffer) {
}

template<typename ISelect, typename IReproduce, typename IGerminate, typename IProduceInitialAgent,
         typename IAddToBuffer, typename IAtCapacity, typename ICommitBuffer>
void srf_stepper<ISelect, IReproduce, IGerminate, IProduceInitialAgent, IAddToBuffer, IAtCapacity,
                 ICommitBuffer>::step() {
    auto progenitors = select_.select();
    for(const auto& progenitor : progenitors) {
        if(at_capacity_.at_capacity())
            break;
        auto seeds = reproduce_.reproduce(progenitor);
        for(const auto& seed : seeds) {
            if(at_capacity_.at_capacity())
                break;
            auto child = germinate_.germinate(seed);
            if(child.has_value())
                add_to_buffer_.add(*child);
        }
    }
    while(!at_capacity_.at_capacity())
        add_to_buffer_.add(produce_initial_agent_.produce());
    commit_buffer_.commit();
}

#endif
