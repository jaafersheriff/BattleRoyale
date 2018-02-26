import os

def main():
   soundFile = open('filesinfolder.txt', 'w')
   filenames = [f for f in os.listdir('.') if os.path.isfile(os.path.join('.', f))]
   for name in filenames:
      soundFile.write(name + '\n')

if __name__ == "__main__":
   main()
