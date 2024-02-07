import torch
import time
import random
from torch.optim import Adam
from torch.utils.data import random_split, DataLoader
from tqdm import tqdm

from data import LichessDataset
from model import BlunderNet, calculate_loss


SEED = 42
random.seed(SEED)
torch.manual_seed(SEED)
torch.cuda.manual_seed(SEED)

device = "cuda" if torch.cuda.is_available() else "cpu"

def train(model, dataloader, optim):
    loss_total = 0
    cnt = 0
    model.train()
    with tqdm(dataloader, unit="batch") as batch_iter:
        for x, policy, value in batch_iter:
            policy_hat, value_hat = model(x.to(device))

            loss = calculate_loss(
                policy_hat.to(device),
                value_hat.to(device),
                policy.to(device),
                value.to(device),
            )
            loss_total += loss.item()
            cnt += 1

            optim.zero_grad()
            loss.backward()
    return loss_total / cnt

def evaluate(model, dataloader):
    loss_total = 0
    cnt = 0
    model.eval()
    with torch.no_grad():
        with tqdm(dataloader, unit="batch") as batch_iter:
            for x, policy, value in batch_iter:
                policy_hat, value_hat = model(x.to(device))

                loss = calculate_loss(
                    policy_hat.to(device),
                    value_hat.to(device),
                    policy.to(device),
                    value.to(device),
                )
                loss_total += loss.item()
                cnt += 1

    return loss_total / cnt


print("loading dataset")
start = time.time()
ld = LichessDataset(["/home/adam/Downloads/lichess_elite/lichess_elite_2020-06.pgn"])
end = time.time()
print(f"dur: {end - start}")

print("splitting dataset and initialising dataloaders")
train_dataset, val_dataset, test_dataset = random_split(ld, [0.8, 0.1, 0.1])
train_dataloader = DataLoader(train_dataset, batch_size=64)
val_dataloader = DataLoader(val_dataset, batch_size=256)
test_dataloader = DataLoader(test_dataset, batch_size=256)

print("creating net")

input_channels = 66
intermediate_channels = 64
n_epochs = 10

net = BlunderNet(
  input_channels=input_channels, intermediate_channels=intermediate_channels
).to(device)
opt = Adam(net.parameters(), lr=0.005, betas=(0.9, 0.999), eps=1e-08)

print("training net")
train_losses = []
val_losses = []
cur_best_loss = float("inf")
for epoch in range(n_epochs):
  print(f"\n\nbeginning epoch {epoch}")
  start = time.time()
  train_loss = train(net, train_dataloader, opt)
  val_loss = evaluate(net, val_dataloader)
  end = time.time()

  print(f"{train_loss=} {val_loss=} duration={end - start}")
  train_losses.append(train_loss)
  val_losses.append(val_loss)
  if val_loss < cur_best_loss:
    cur_best_loss = val_loss
    torch.save(net.state_dict(), "supervised_learning_model.pt")

test_loss = evaluate(net, test_dataloader)
print(f"{test_loss=}")