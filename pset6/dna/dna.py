import sys
import csv


def find_match(max_repeats: dict, database: list) -> str:
    # find the matching person from the DNA DB
    for candidate in database:
        found = True
        for str_seq, num in max_repeats.items():
            if num != candidate[str_seq]:
                found = False
                break
        if found:
            return candidate["name"]

    return "No match"


def max_num_repeats(full_seq: str, str_seq: str) -> int:
    # Calculate longest repeats per STR
    curr_repeats = max_repeats = 0

    len_str_seq = len(str_seq)
    len_full_seq = len(full_seq)

    # possible corner case
    if len(str_seq) > len(full_seq):
        return repeats

    # find the first match, then check how many there are back to back
    start_idx = 0

    while (start_idx < len_full_seq - len_str_seq + 1):
        if full_seq[start_idx:start_idx + len_str_seq] == str_seq:
            # found
            curr_repeats += 1
            max_repeats = max(max_repeats, curr_repeats)
            start_idx += len_str_seq  # how str_len ahead
        else:
            curr_repeats = 0
            start_idx += 1  # slide to next char

    return max_repeats


def main():
    if len(sys.argv) != 3:
        print("Usage: python dna.py data.csv sequence.txt")
        return

    db = []
    with open(sys.argv[1]) as f:
        reader = csv.DictReader(f)
        for row in reader:
            # turn values to ints
            for key in row.keys():
                if key == "name":
                    continue
                row[key] = int(row[key])
            db.append(row)

    with open(sys.argv[2]) as f:
        seq = f.readline()

    # print(db)
    # infer the STRs from database
    str_seqs = [key for key in db[0].keys() if key != "name"]

    # dict to store the max repeats per STR
    max_repeats = {}
    for str_seq in str_seqs:
        max_repeats[str_seq] = max_num_repeats(seq, str_seq)

    # print(max_repeats)
    print(find_match(max_repeats, db))


if __name__ == "__main__":
    main()