import glob
import json
import csv

from os.path import basename


transcriptions = {}
with open('transcriptions.tsv', 'r') as csvfile:
    csvreader = csv.reader(csvfile, delimiter='\t')
    for row in csvreader:
        transcription = row[0].decode('utf-8').encode('utf-8')
        word = row[1].decode('utf-8').encode('utf-8')
        transcriptions[word] = transcription

words = []
json_words = []
for directory in glob.glob('./splitted_records/*'):
    for record_file in glob.glob(directory + '/*.wav'):
        record_file = record_file.decode('utf-8').encode('utf-8')
        word = basename(record_file).replace('.wav', '')
        if word not in words:
            words.append(word)
            json_words.append({
                'spelling': word,
                'transcription': transcriptions[word],
                'utterances': []
            })
        for entry in json_words:
            if entry.get('spelling') != word:
                continue
            entry['utterances'].append(record_file)
            entry['utterances'].sort()

print json.dumps(json_words, ensure_ascii=False,
                 indent=2, separators=(',', ': '))