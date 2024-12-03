from setuptools import setup
import os
from glob import glob

package_name = 'mock_hardware_interface'

setup(
    name=package_name,
    # ... other setup parameters ...
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
    ],
    # ... other setup parameters ...
)
