import os
import cv2
import numpy as np 
import torch
from torch.utils.data import Dataset, DataLoader, random_split
from PIL import Image
from torchvision import transforms
import random


class DigitDataset(Dataset):
    
    def __init__(self, data_dir, transform, is_training=True):
        self.data_dir = data_dir
        self.transform = transform
        self.is_training = is_training
        self.image_paths = []
        self.labels = []

        for label in range(10):
            label_dir = os.path.join(data_dir, str(label))
            if os.path.exists(label_dir):
                for img_name in os.listdir(label_dir):
                    if img_name.endswith(('.jpg', '.jpeg', '.png')):
                        self.image_paths.append(os.path.join(label_dir, img_name))
                        self.labels.append(label)

        print(f"一共加载{len(self.image_paths)}张图片")

    def __len__(self):
        return len(self.image_paths)

    def __getitem__(self, idx):
        img_path = self.image_paths[idx]
        label = self.labels[idx]

        image = cv2.imread(img_path)
        if image is None:
            raise ValueError(f"无法读取图像：{img_path}")

        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        image = Image.fromarray(image)
        if self.transform:
            image = self.transform(image)
        
        return image, label
    
    def get_transform():
        return transforms.Compose([
                transforms.Grayscale(num_output_channels=1),  # 转为灰度图
                transforms.Resize((32, 32)),  # 调整大小
                transforms.ToTensor(),  # 转换为tensor
                transforms.Normalize((0.1307,), (0.3081,)) # 
            ])

    
    def load_data(data_dir, batch_size=64, val_split=0.2, random_seed=100):

        # 固定随机种子
        torch.manual_seed(random_seed)
        np.random.seed(random_seed)
        random.seed(random_seed)

        transform = get_transform()
        full_dataset = DigitDataset(data_dir, transform=transform)
        total_size = len(full_dataset)
        val_size = int(total_size * val_split)
        train_size = total_size - val_size
        train_dataset, val_dataset = random_split(full_dataset, [train_size, val_size])


        train_loader = DataLoader(
            train_dataset,
            batch_size=batch_size,
            shuffle=True,
            num_workers=4,
            pin_memory=True
        )
        val_loader = DataLoader(
            val_dataset,
            batch_size=batch_size,
            shuffle=False,
            num_workers=4,
            pin_memory=True
        )
        print(f"训练集大小：{train_size}, 验证集大小：{val_size}")
        return train_loader, val_loader




