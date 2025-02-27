from datetime import datetime

def fibonacci (n):
    if n < 0: raise
    if n == 0: return 0
    if n == 1: return 1
    return fibonacci(n - 1) + fibonacci(n - 2)


def main():
    # n = 10
    # n = 40
    n = 60
    startTime = datetime.now()
    print(f'fibonacci({n})={fibonacci(n)}')
    endTime = datetime.now()
    seconds = endTime - startTime
    print(f'time:{seconds}')
