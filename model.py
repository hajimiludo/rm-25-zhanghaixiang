# best_model.py - 轻量级高效模型，适合嵌入式部署
import torch
import torch.nn as nn

class DigitNet(nn.Module):
    def __init__(self, num_classes=10):
        super(DigitNet, self).__init__()
        
        # 第一卷积块：标准卷积
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, padding=1),
            nn.BatchNorm2d(16),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2)  # 32x32 -> 16x16
        )
        
        # 第二卷积块：深度可分离卷积
        self.conv2_dw = nn.Sequential(
            nn.Conv2d(16, 16, kernel_size=3, padding=1, groups=16),  # 深度卷积
            nn.BatchNorm2d(16),
            nn.ReLU(inplace=True)
        )
        self.conv2_pw = nn.Sequential(
            nn.Conv2d(16, 32, kernel_size=1),  # 逐点卷积
            nn.BatchNorm2d(32),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2)  # 16x16 -> 8x8
        )
        
        # 第三卷积块：标准卷积
        self.conv3 = nn.Sequential(
            nn.Conv2d(32, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(2)  # 8x8 -> 4x4
        )
        
        # 注意力机制
        self.attention = nn.Sequential(
            nn.AdaptiveAvgPool2d(1),
            nn.Conv2d(64, 4, kernel_size=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(4, 64, kernel_size=1),
            nn.Sigmoid()
        )
        
        # 全连接层
        self.fc = nn.Sequential(
            nn.Linear(64 * 4 * 4, 128),
            nn.ReLU(inplace=True),
            nn.Dropout(0.5),
            nn.Linear(128, num_classes)
        )
    
    def forward(self, x):
        """
        前向传播
        """
        # 第一卷积块
        x = self.conv1(x)  # [batch, 16, 16, 16]
        
        # 第二卷积块（深度可分离卷积）
        x = self.conv2_dw(x)  # [batch, 16, 16, 16]
        x = self.conv2_pw(x)  # [batch, 32, 8, 8]
        
        # 第三卷积块
        x = self.conv3(x)  # [batch, 64, 4, 4]
        
        # 注意力机制
        att = self.attention(x)  # [batch, 64, 1, 1]
        x = x * att  # 应用注意力权重
        
        # 展平
        x = x.view(x.size(0), -1)  # [batch, 64*4*4=1024]
        
        # 全连接层
        x = self.fc(x)  # [batch, num_classes]
        
        return x

# 测试代码
if __name__ == "__main__":
    model = DigitNet()
    print(model)
    
    # 测试前向传播
    test_input = torch.randn(1, 1, 32, 32)  # [batch, channels, height, width]
    output = model(test_input)
    print(f"输入形状: {test_input.shape}")
    print(f"输出形状: {output.shape}")
    
    # 统计参数量
    total_params = sum(p.numel() for p in model.parameters())
    print(f"总参数量: {total_params:,}")