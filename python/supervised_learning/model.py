import torch.nn as nn
import torch.nn.functional as F

def calculate_loss(policy_hat, value_hat, policy, value):
  policy_loss = F.cross_entropy(policy_hat, policy.squeeze(1))
  value_loss = F.mse_loss(value_hat.float().squeeze(1), value.float())
  return policy_loss + value_loss

class InputBlock(nn.Module):
  def __init__(self, input_channels, output_channels):
    super().__init__()
    self.conv = nn.Conv2d(input_channels, output_channels, kernel_size=3, padding=1)
    self.bn = nn.BatchNorm2d(output_channels)
    self.relu = nn.ReLU()
  
  def forward(self, x):
    out = self.bn(self.conv(x))
    return self.relu(out)

class ResidualBlock(nn.Module):
  # NOTE: I very likely will replace this with Squeeze-and-Excitation blocks but this will do for now
  def __init__(self, n_channels):
    super().__init__()
    self.conv = nn.Conv2d(n_channels, n_channels, kernel_size=3, padding=1)
    self.bn = nn.BatchNorm2d(n_channels)
    self.relu = nn.ReLU()

  def forward(self, x):
    residual = x
    out = self.bn(self.conv(x))
    out = self.relu(out)
    return out + residual

class PolicyHead(nn.Module):
  def __init__(self, input_channels):
    super().__init__()
    self.conv = nn.Conv2d(input_channels, 2, kernel_size=1)
    self.bn = nn.BatchNorm2d(2)
    self.relu = nn.ReLU()
    self.fc = nn.Linear(64*2, 64*64 + (2 * 8 * 8 * 4)) # source * dest + promotions
    # softmax is done in loss func I think

  def forward(self, x):
    out = self.bn(self.conv(x))
    out = self.relu(out)
    return self.fc(out.view(x.shape[0], -1)) # should be auto-flattened

class ValueHead(nn.Module):
  def __init__(self, input_channels):
    super().__init__()
    self.conv = nn.Conv2d(input_channels, 1, kernel_size=1)
    self.bn = nn.BatchNorm2d(1)
    self.relu1 = nn.ReLU()
    self.fc1 = nn.Linear(64, 256)
    self.relu2 = nn.ReLU()
    self.fc2 = nn.Linear(256, 1)
    self.tanh = nn.Tanh()

  def forward(self, x):
    out = self.bn(self.conv(x))
    out = self.relu1(out)
    out = self.relu2(self.fc1(out.view(x.shape[0], -1)))
    out = self.tanh(self.fc2(out))
    return out

class BlunderNet(nn.Module):
  def __init__(self, input_channels, intermediate_channels):
    super().__init__()
    self.input_block = InputBlock(input_channels, intermediate_channels)
    self.main_trunk = nn.ModuleList([ResidualBlock(intermediate_channels)] * 8)
    self.policy_head = PolicyHead(intermediate_channels)
    self.value_head = ValueHead(intermediate_channels)

  def forward(self, x):
    out = self.input_block(x)
    for res_block in self.main_trunk:
      out = res_block(out)
    policy = self.policy_head(out)
    value = self.value_head(out)
    return (policy, value)