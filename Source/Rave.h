#pragma once

#include <torch/torch.h>
#include <torch/script.h>

struct RAVE {
  
  torch::jit::Module model;

  int sr;
  int decode_explosion;
  int z_per_second;
  int prior_temp_size;
    
  std::vector<torch::jit::IValue> inputs_rave;

  RAVE() {
//    torch::init_num_threads();
//    unsigned int num_threads = std::thread::hardware_concurrency();
//
//    if (num_threads > 0) {
//        torch::set_num_threads((int)num_threads);
//        torch::set_num_interop_threads((int)num_threads);
//    } else {
//        torch::set_num_threads(4);
//        torch::set_num_interop_threads(4);
//    }
    
    torch::jit::getProfilingMode() = false;
    torch::NoGradGuard no_grad;
    torch::jit::setGraphExecutorOptimize(true);
    }
    
  void load_model(const std::string& rave_model_file) {
    try {
        torch::NoGradGuard no_grad;
        this->model = torch::jit::load(rave_model_file);
    }
    catch (const c10::Error& e) {
        std::cerr << e.what();
        std::cerr << e.msg();
        std::cerr << "error loading the model\n";
    }

    this->sr = 48000;
    this->decode_explosion = 2048;
    this->z_per_second = (int)48000/2048;
    this->prior_temp_size = 512;

    auto named_buffers = this->model.named_buffers();
    for (auto const& i: named_buffers) {
        if (i.name == "_rave.latent_size") {
            std::cout<<i.name<<std::endl;
            std::cout << i.value << std::endl;
        }
        if (i.name == "_rave.decode_params") {
            std::cout<<i.name<<std::endl;
            std::cout << i.value << std::endl;
            this->decode_explosion = i.value[1].item<int>();
        }
        if (i.name == "_rave.sampling_rate") {
            std::cout<<i.name<<std::endl;
            std::cout << i.value << std::endl;
            this->sr = i.value.item<int>();
        }
        if (i.name == "_prior.previous_step") {
            std::cout<<i.name<<std::endl;
            std::cout << i.value.sizes()[1] << std::endl;
            this->prior_temp_size = (int) i.value.sizes()[1];
        }

        inputs_rave.clear();
        inputs_rave.push_back(torch::ones({1,1,decode_explosion}));
    }
  }
  
  torch::Tensor sample_from_prior (const float temperature) {
    torch::NoGradGuard no_grad;

    inputs_rave[0] = torch::ones({1,1,1}) * temperature;
    const auto prior = this->model.get_method("prior")(inputs_rave).toTensor();

    inputs_rave[0] = prior;
    const auto y = this->model.get_method("decode")(inputs_rave).toTensor();

    return y.squeeze(0); // remove batch dim
  }

  torch::Tensor encode_decode (torch::Tensor input) {
    torch::NoGradGuard no_grad;

    inputs_rave[0] = input;
    const auto y = this->model(inputs_rave).toTensor();

    return y.squeeze(0); // remove batch dim

  }

};
