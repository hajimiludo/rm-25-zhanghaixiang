# train.py - 基础训练脚本
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
import time
from tqdm import tqdm
import matplotlib.pyplot as plt

from model import DigitNet
from process import load_data


def train_epoch(model, train_loader, criterion, optimizer, device, epoch):
    """
    训练一个epoch
    """
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0
    
    pbar = tqdm(train_loader, desc=f'Epoch {epoch+1} [Train]')
    for images, labels in pbar:
        images, labels = images.to(device), labels.to(device)
        
        # 前向传播
        optimizer.zero_grad()
        outputs = model(images)
        loss = criterion(outputs, labels)
        
        # 反向传播
        loss.backward()
        optimizer.step()
        
        # 统计
        running_loss += loss.item()
        _, predicted = outputs.max(1)
        total += labels.size(0)
        correct += predicted.eq(labels).sum().item()
        
        # 更新
        pbar.set_postfix({
            'loss': running_loss / (total / train_loader.batch_size),
            'acc': 100. * correct / total
        })
    
    train_loss = running_loss / len(train_loader)
    train_acc = 100. * correct / total
    return train_loss, train_acc

def validate(model, val_loader, criterion, device, epoch):
    """
    验证模型
    """
    model.eval()
    running_loss = 0.0
    correct = 0
    total = 0
    
    with torch.no_grad():
        pbar = tqdm(val_loader, desc=f'Epoch {epoch+1} [Val]')
        for images, labels in pbar:
            images, labels = images.to(device), labels.to(device)
            
            # 前向传播
            outputs = model(images)
            loss = criterion(outputs, labels)
            
            # 统计
            running_loss += loss.item()
            _, predicted = outputs.max(1)
            total += labels.size(0)
            correct += predicted.eq(labels).sum().item()
            
            # 更新进度条
            pbar.set_postfix({
                'loss': running_loss / (total / val_loader.batch_size),
                'acc': 100. * correct / total
            })
    
    val_loss = running_loss / len(val_loader)
    val_acc = 100. * correct / total
    
    return val_loss, val_acc

def train_model(model, train_loader, val_loader):
    """
    训练模型
    """
    use_cuda = True
    learning_rate = 0.001
    weight_decay = 1e-4
    epochs = 20

    device = torch.device('cuda' if torch.cuda.is_available() and use_cuda else 'cpu')
    print(f"使用设备: {device}")
    
    model = model.to(device)
    
    # 损失函数和优化器
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate, weight_decay=weight_decay)
    scheduler = optim.lr_scheduler.ReduceLROnPlateau(optimizer, 'min', patience=3, factor=0.5)
    
    # 训练历史
    train_losses = []
    train_accs = []
    val_losses = []
    val_accs = []
    best_val_acc = 0.0
    
    print(f"开始训练 {model.get_name()} 模型...")
    print(f"总epochs: {epochs}, 批次大小: {train_loader.batch_size}")
    
    start_time = time.time()
    
    for epoch in range(epochs):
        # 训练
        train_loss, train_acc = train_epoch(model, train_loader, criterion, optimizer, device, epoch)
        
        # 验证
        val_loss, val_acc = validate(model, val_loader, criterion, device, epoch)
        
        # 学习率调度
        scheduler.step(val_loss)
        
        # 记录历史
        train_losses.append(train_loss)
        train_accs.append(train_acc)
        val_losses.append(val_loss)
        val_accs.append(val_acc)
        
        # 保存最佳模型
        if val_acc > best_val_acc:
            best_val_acc = val_acc
            torch.save({
                'epoch': epoch + 1,
                'model_state_dict': model.state_dict(),
                'optimizer_state_dict': optimizer.state_dict(),
                'val_acc': val_acc,
                'val_loss': val_loss,
            }, f'best_{model.get_name().lower()}.pth')
            print(f"保存最佳模型，验证准确率: {val_acc:.2f}%")
        
        print(f'Epoch {epoch+1}/{epochs}:')
        print(f'  Train Loss: {train_loss:.4f}, Train Acc: {train_acc:.2f}%')
        print(f'  Val Loss: {val_loss:.4f}, Val Acc: {val_acc:.2f}%')
        print(f'  Best Val Acc: {best_val_acc:.2f}%')
    
    training_time = time.time() - start_time
    print(f"训练完成! 总耗时: {training_time:.2f}秒")
    print(f"最佳验证准确率: {best_val_acc:.2f}%")
    
    # 绘制训练曲线
    plot_training_history(train_losses, train_accs, val_losses, val_accs, model.get_name())
    
    return best_val_acc

def plot_training_history(train_losses, train_accs, val_losses, val_accs, model_name):
    """
    绘制训练历史曲线
    """
    plt.figure(figsize=(12, 4))
    
    # 损失曲线
    plt.subplot(1, 2, 1)
    plt.plot(train_losses, label='Train Loss')
    plt.plot(val_losses, label='Val Loss')
    plt.title(f'{model_name} - Loss')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.legend()
    
    # 准确率曲线
    plt.subplot(1, 2, 2)
    plt.plot(train_accs, label='Train Acc')
    plt.plot(val_accs, label='Val Acc')
    plt.title(f'{model_name} - Accuracy')
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy (%)')
    plt.legend()
    
    plt.tight_layout()
    plt.savefig(f'{model_name.lower()}_training_history.png')
    print(f"保存训练历史图: {model_name.lower()}_training_history.png")

def main():
    # 加载数据
    train_loader, val_loader = load_data(batch_size=64)
    
    model = DigitNet()

    # 训练模型
    best_acc = train_model(model, train_loader, val_loader)

    torch.save(model.state_dict(), 'final_digitnet.pth')
    print("保存最终模型: final_digitnet.pth")

    print("训练完成!")


if __name__ == "__main__":
    main()