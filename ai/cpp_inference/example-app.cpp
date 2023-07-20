#include <torch/script.h> // One-stop header.
#include <torch/torch.h> // For torch::Tensor

#include <iostream>
#include <memory>

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: example-app <path-to-exported-script-module>\n";
    return -1;
  }


  torch::jit::script::Module module;
  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(argv[1]);

    torch::Tensor inp = torch::rand({1, 3, 480, 480});

    torch::Tensor output = module.forward({inp}).toTensor();
    std::cout << output.size(0) << " " << output.size(1) << std::endl;
    std::cout << output << std::endl;
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    return -1;
  }

  std::cout << "ok\n";
}