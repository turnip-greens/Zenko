#include "CardEngine.h"

namespace Zenko {

    // ── Date Helper ───────────────────────────────────────
    // Returns how many days have passed since Jan 1 2000.
    // We use this as our internal "today" timestamp so dates
    // are just plain integers instead of messy DateTime objects.
    int CardEngine::Today() {
        DateTime epoch(2000, 1, 1);
        TimeSpan diff = DateTime::Today - epoch;
        return diff.Days;
    }

    // ── CSV Helpers ───────────────────────────────────────
    // Trims whitespace and handles cells that have two lines:
    // line 1 = Japanese text, line 2 = romaji.
    // Returns them combined as "Japanese (romaji)".
    // If the cell is empty or literally "n/a" we return "n/a".
    String^ CardEngine::CleanCell(String^ cell) {
        cell = cell->Trim();
        if (cell == L"n/a" || cell->Length == 0) return L"n/a";
        auto lines = cell->Split('\n');
        String^ japanese = "", ^ romaji = "";
        for each (String ^ line in lines) {
            String^ t = line->Trim()->Replace("\r", "");
            if (t->Length == 0) continue;
            if (japanese->Length == 0) japanese = t;
            else romaji = t;
        }
        return romaji->Length > 0 ? japanese + " (" + romaji + ")" : japanese;
    }

    // Parses a raw CSV string into a list of rows, where each
    // row is a list of cell strings. Handles quoted cells that
    // contain commas or newlines, and escaped quotes ("").
    List<List<String^>^>^ CardEngine::ParseCSV(String^ raw) {
        auto rows = gcnew List<List<String^>^>();
        auto currentRow = gcnew List<String^>();
        auto currentCell = gcnew System::Text::StringBuilder();
        bool inQuotes = false;

        for (int i = 0; i < raw->Length; i++) {
            wchar_t c = raw[i];
            if (c == '"') {
                // "" inside quotes = escaped quote character
                if (inQuotes && i + 1 < raw->Length && raw[i + 1] == '"') {
                    currentCell->Append('"'); i++;
                }
                else inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes) {
                currentRow->Add(currentCell->ToString());
                currentCell->Clear();
            }
            else if (c == '\n' && !inQuotes) {
                currentRow->Add(currentCell->ToString());
                currentCell->Clear();
                if (currentRow->Count > 0) rows->Add(currentRow);
                currentRow = gcnew List<String^>();
            }
            else if (c == '\r') continue;
            else currentCell->Append(c);
        }
        // Don't forget the last row if file doesn't end with newline
        if (currentRow->Count > 0) {
            currentRow->Add(currentCell->ToString());
            rows->Add(currentRow);
        }
        return rows;
    }

    // ── LoadCSV ───────────────────────────────────────────
    // Reads the CSV file and builds the card list.
    // Expected columns: kanji, meaning, onyomi, kunyomi
    // The first row is a header and gets skipped.
    // Each card starts with a fresh CardProgress (due immediately).
    void CardEngine::LoadCSV(String^ path) {
        cards = gcnew List<Card^>();
        String^ raw = System::IO::File::ReadAllText(path, System::Text::Encoding::UTF8);
        auto rows = ParseCSV(raw);
        bool first = true;

        for each (auto row in rows) {
            if (first) { first = false; continue; }  // skip header
            if (row->Count < 2) continue;             // skip empty/bad rows

            CardData^ data = gcnew CardData();
            data->kanji = CleanCell(row->Count > 0 ? row[0] : "");
            data->meaning = CleanCell(row->Count > 1 ? row[1] : "");
            data->onyomi = CleanCell(row->Count > 2 ? row[2] : "");
            data->kunyomi = CleanCell(row->Count > 3 ? row[3] : "");

            // Unicode codepoint of the first character — used to
            // build the stroke image filename e.g. u6c34.png
            if (data->kanji->Length > 0)
                data->unicodePoint = (int)data->kanji[0];

            cards->Add(gcnew Card(data));
        }
    }

    // ── SaveProgress ──────────────────────────────────────
    // Writes SRS state for every card to a binary file.
    // Each record is exactly 24 bytes in this order:
    //   unicodePoint (int32), interval (int32),
    //   easeFactor (float32), dueDate (int32),
    //   reviewCount (int32), lapses (int32)
    void CardEngine::SaveProgress(String^ path) {
        auto writer = gcnew System::IO::BinaryWriter(
            System::IO::File::Open(path, System::IO::FileMode::Create));
        try {
            for each (Card ^ card in cards) {
                writer->Write(card->data->unicodePoint);
                writer->Write(card->progress->interval);
                writer->Write(card->progress->easeFactor);
                writer->Write(card->progress->dueDate);
                writer->Write(card->progress->reviewCount);
                writer->Write(card->progress->lapses);
            }
        }
        finally {
            writer->Close();
        }
    }

    // ── LoadProgress ──────────────────────────────────────
    // Reads the binary file and matches each record back to a
    // card by unicodePoint. If a card isn't found (e.g. you
    // added new cards to the CSV) it just gets a fresh progress.
    void CardEngine::LoadProgress(String^ path) {
        if (!System::IO::File::Exists(path)) return; // first run, nothing to load

        auto reader = gcnew System::IO::BinaryReader(
            System::IO::File::Open(path, System::IO::FileMode::Open));
        try {
            while (reader->BaseStream->Position < reader->BaseStream->Length) {
                int   unicode = reader->ReadInt32();
                int   interval = reader->ReadInt32();
                float easeFactor = reader->ReadSingle();
                int   dueDate = reader->ReadInt32();
                int   reviewCount = reader->ReadInt32();
                int   lapses = reader->ReadInt32();

                // Find the matching card and apply the saved progress
                for each (Card ^ card in cards) {
                    if (card->data->unicodePoint == unicode) {
                        card->progress->interval = interval;
                        card->progress->easeFactor = easeFactor;
                        card->progress->dueDate = dueDate;
                        card->progress->reviewCount = reviewCount;
                        card->progress->lapses = lapses;
                        break;
                    }
                }
            }
        }
        finally {
            reader->Close();
        }
    }

    // ── GetDueCards ───────────────────────────────────────
    // Returns every card whose dueDate is today or in the past.
    // These are the cards the player should study this session.
    List<Card^>^ CardEngine::GetDueCards() {
        auto due = gcnew List<Card^>();
        int today = Today();
        for each (Card ^ card in cards) {
            if (card->progress->dueDate <= today)
                due->Add(card);
        }
        return due;
    }

    // ── DueCount ──────────────────────────────────────────
    int CardEngine::DueCount() {
        return GetDueCards()->Count;
    }

    // ── UpdateProgress ────────────────────────────────────
    // SM-2 spaced repetition algorithm.
    // q = 0 (wrong), 3 (correct), or 5 (easy)
    //
    // Wrong:   reset interval to 1, count as a lapse
    // Correct: grow interval by easeFactor, adjust ease up/down
    //          based on how confident the answer was
    //
    // easeFactor floor is 1.3 — below that cards would never
    // space out and you'd just see them every day forever.
    void CardEngine::UpdateProgress(Card^ card, Grade grade) {
        int q = (int)grade;
        CardProgress^ p = card->progress;

        if (q < 3) {
            // Wrong answer — reset back to the start
            p->interval = 1;
            p->lapses++;
        }
        else {
            // Correct — calculate next interval
            if (p->reviewCount == 0)      p->interval = 1;
            else if (p->reviewCount == 1) p->interval = 6;
            else p->interval = (int)Math::Round(p->interval * p->easeFactor);

            // Adjust ease: q=3 → no change, q=5 → +0.2, q=2 → -0.2
            p->easeFactor += 0.1f * (q - 3);

            // Never let ease drop below 1.3
            if (p->easeFactor < 1.3f) p->easeFactor = 1.3f;
        }

        p->dueDate = Today() + p->interval;
        p->reviewCount++;
    }
} 