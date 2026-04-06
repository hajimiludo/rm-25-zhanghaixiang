import torch.nn as nn
import torch.nn.functional as F

class LeNet(nn.Module):
    """
    LeNet-5 模型
    适用于手写数字识别,输入尺寸为32x32
    """
    def __init__(self, num_classes=10):
        super(LeNet, self).__init__()
        # 卷积层部分
        self.conv1 = nn.Conv2d(1, 6, kernel_size=5)  # 输入通道1，输出通道6，5x5卷积核
        self.pool = nn.AvgPool2d(kernel_size=2, stride=2)  # 平均池化，2x2窗口，步长为2
        self.conv2 = nn.Conv2d(6, 16, kernel_size=5)
        
        # 全连接层部分
        self.fc1 = nn.Linear(16*5*5, 120)  # 展平后的特征维度：16*5*5
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, num_classes)  # 输出层，默认10个类别（0-9）

    def forward(self, x):
        # 卷积层 + 激活函数 + 池化
        x = self.pool(F.relu(self.conv1(x)))  # 输出: (6, 14, 14)
        x = self.pool(F.relu(self.conv2(x)))  # 输出: (16, 5, 5)
        
        # 展平特征图
        x = x.view(x.size(0), -1)  # 展平为 (batch_size, 16*5*5)
        
        # 全连接层
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)  # 最终输出
        return x
    
import torch    
X = torch.rand(1,1,32,32)
print(X.shape)
print(X)
model = LeNet()
print(model)
Y = model.forward(X)
print(Y)
num = torch.argmax(Y, dim=1)
print(num.item())