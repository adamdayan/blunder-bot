import torch

from model import BlunderNet

input_channels = 66
intermediate_channels = 64
model_path = "/home/adam/Downloads/supervised_learning_model.pt"

net = BlunderNet(
  input_channels=input_channels, intermediate_channels=intermediate_channels
)

net.load_state_dict(torch.load(model_path, map_location=torch.device("cpu")))
scripted_net = torch.jit.script(net)
scripted_net.save("/home/adam/Downloads/scripted_supervised_learning_model.pt")