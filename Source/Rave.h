#pragma once

#include <torch/torch.h>
#include <torch/script.h>
//#include <torch/csrc/jit/runtime/graph_executor.h>
//#include <torch/csrc/jit/serialization/export.h>
//#include <tuple>
//#include <assert.h>

struct RAVE {
  
  torch::jit::Module model;

  unsigned int sr;
  unsigned int decode_explosion;
  unsigned int z_per_second;
    
  std::vector<torch::jit::IValue> inputs_rave_decode;

  RAVE(std::string rave_model_file) {
    torch::init_num_threads();
    unsigned int num_threads = std::thread::hardware_concurrency();
     
    if (num_threads > 0) {
        torch::set_num_threads((int)num_threads);
        torch::set_num_interop_threads((int)num_threads);
    } else {
        torch::set_num_threads(4);
        torch::set_num_interop_threads(4);
    }
    
    torch::jit::getProfilingMode() = false;
    torch::NoGradGuard no_grad;
    // std::cout << "Executor optimize: " << torch::jit::getGraphExecutorOptimize() << std::endl;
     torch::jit::setGraphExecutorOptimize(true);
    // std::cout << torch::jit::getGraphExecutorOptimize() << std::endl;
    // torch::autograd::profiler::RecordProfile guard("istft.trace.json");
//     torch::jit::getBailoutDepth() = 1;
    // std::cout << torch::get_num_interop_threads() << std::endl;
    // std::cout << torch::get_num_threads() << std::endl;
    // std::cout << torch::get_parallel_info() << std::endl;
    // std::cout << torch::get_cxx_flags() << std::endl;

    this->model = torch::jit::load(rave_model_file);
    this->sr = 48000;
    this->decode_explosion = 2048; // TODO: How to read this from model?
    this->z_per_second = (int)48000/2048;
      
    inputs_rave_decode.push_back(torch::ones({1,1,decode_explosion}));
    
  }
  
  torch::Tensor sample_from_prior (unsigned int seconds, float temperature) {

    std::vector<torch::jit::IValue> inputs_rave_prior;
    inputs_rave_prior.push_back(torch::ones({1,1,1}) * temperature);
    const auto prior = this->model.get_method("prior")(inputs_rave_prior).toTensor();

    inputs_rave_decode[0] = prior;
    const auto y = this->model.get_method("decode")(inputs_rave_decode).toTensor();

    return y; // remove batch dim
  }

  torch::Tensor encode_decode (torch::Tensor input) {
    std::vector<torch::jit::IValue> inputs_rave;
    inputs_rave.push_back(input);
    const auto y = this->model(inputs_rave).toTensor();

    return y.squeeze(0); // remove batch dim

  }

};
