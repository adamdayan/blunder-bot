import torch
import time
import random
from torch.optim import Adam
from torch.utils.data import random_split, DataLoader
from tqdm import tqdm

from data import LichessDataset
from model import BlunderNet, calculate_loss, calculate_policy_accuracy


SEED = 42
random.seed(SEED)
torch.manual_seed(SEED)
torch.cuda.manual_seed(SEED)

device = "cuda" if torch.cuda.is_available() else "cpu"
print(f"running on {device}")

def train(model, dataloader, optim):
    loss_total = 0
    batch_cnt = 0
    example_cnt = 0
    accuracy = 0
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
            batch_cnt += 1

            accuracy = ((calculate_policy_accuracy(policy_hat, policy) * policy_hat.shape[0]) + (accuracy * example_cnt)) / (policy_hat.shape[0] + example_cnt)
            example_cnt += policy_hat.shape[0]

            optim.zero_grad()
            loss.backward()
            optim.step()
    return (loss_total / batch_cnt) , accuracy

def evaluate(model, dataloader):
    loss_total = 0
    batch_cnt = 0
    example_cnt = 0
    accuracy = 0
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
                batch_cnt += 1

                accuracy = ((calculate_policy_accuracy(policy_hat, policy) * policy_hat.shape[0]) + (accuracy * example_cnt)) / (policy_hat.shape[0] + example_cnt)
                example_cnt += policy_hat.shape[0]

    return (loss_total / batch_cnt), accuracy


print("loading dataset")
start = time.time()

dataset_root_path = "/home/adam/Downloads/lichess_elite/"
# dataset_root_path = "/root/data/"
train_dataset = LichessDataset([dataset_root_path + path for path in ["lichess_elite_2020-05.pgn", "lichess_elite_2020-04.pgn", "lichess_elite_2020-03.pgn"]])
val_dataset = LichessDataset([dataset_root_path + path for path in ["lichess_elite_2020-06.pgn"]])
test_dataset = LichessDataset([dataset_root_path + path for path in ["lichess_elite_2023-07.pgn"]])

end = time.time()
print(f"dur: {end - start}")

# TODO: delete
# for i, d in enumerate(train_dataset):
#     if i >= 2:
#         print(d[1])
#         break
    # print(d[0][6])

# exit()

print("splitting dataset and initialising dataloaders")
train_dataloader = DataLoader(train_dataset, batch_size=1024, pin_memory=True, num_workers=3)
val_dataloader = DataLoader(val_dataset, batch_size=1024, pin_memory=True, num_workers=1)
test_dataloader = DataLoader(test_dataset, batch_size=1024, pin_memory=True, num_workers=1)

print("creating net")

input_channels = 66
intermediate_channels = 64
n_epochs = 5
model_path = "supervised_learning_model.pt"

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
    train_loss, train_accuracy = train(net, train_dataloader, opt)
    val_loss, val_accuracy = evaluate(net, val_dataloader)
    end = time.time()

    print(f"{train_loss=:.2f} {val_loss=:.2f}")
    print(f"train_accuracy={train_accuracy * 100:.2f}% val_accuracy={val_accuracy * 100:.2f}%")
    print(f"duration={end - start:.2f}")

    train_losses.append(train_loss)
    val_losses.append(val_loss)
    if val_loss < cur_best_loss:
        cur_best_loss = val_loss
        net.eval()
        torch.save(net.state_dict(), model_path)

test_loss, test_accuracy = evaluate(net, test_dataloader)
print(f"{test_loss=}")

net.load_state_dict(torch.load(model_path))
scripted_net = torch.jit.script(net.to("cpu"))
scripted_net.save("scripted_" + model_path)