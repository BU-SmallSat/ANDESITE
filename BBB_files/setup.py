from setuptools import setup, find_packages, Extension

# makes __version__ a local variable
exec(open('MULE/_version.py').read())
# http://python-packaging.readthedocs.org/en/latest/command-line-scripts.html


setup(name='MULE',
      version=__version__,
      packages=find_packages(),
      package_data={
          'core': ['resources/*',
                   'resources/tone_in_noise/*']
      },
      scripts=['scripts/mule'],
      #url='',
      #license='',
      #ext_modules=cythonize(extensions),
      #include_dirs=[np.get_include()],
      #author='',
      #author_email='',
      #description=''
      install_requires=[
          'docopt >= 0.6',
          'crc16',
          'Adafruit_BBIO',
          'pyserial',
          'pexpect'
      ],
      tests_require=[
          'pytest',
          'hypothesis'
      ],
      test_suite="py.test")