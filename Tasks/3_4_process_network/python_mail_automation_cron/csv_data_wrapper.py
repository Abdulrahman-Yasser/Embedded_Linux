import csv
def get_elements_of_csv():
    a = []
    with open('jobs_data.csv', mode ='r')as file:
        csvFile = csv.reader(file, delimiter=',')
        for lines in csvFile:
            a.append(lines)
    print(a)
    return a

