#pragma once
#pragma once
#include <windows.h>

namespace Zenko {

    using namespace System;
    using namespace System::Collections::Generic;

    // ── SRS Grade ─────────────────────────────────────────
    // Pass this to UpdateProgress after each answer
    public enum class Grade {
        Wrong = 0,      // completely wrong
        Correct = 3,    // correct but slow/unsure
        Easy = 5        // correct and confident
    };

    // ── Card Data ─────────────────────────────────────────
    // Loaded from CSV, never modified at runtime
    public ref struct CardData {
        String^ kanji;
        String^ meaning;
        String^ onyomi;
        String^ kunyomi;
        int     unicodePoint;   // for stroke image lookup

        CardData() : unicodePoint(0) {}
    };

    // ── Card Progress ──────────────────────────────────────
    // SRS state, saved to progress.bin
    public ref struct CardProgress {
        int   interval;     // days until next review
        float easeFactor;   // SM-2 ease, min 1.3
        int   dueDate;      // days since Jan 1 2000
        int   reviewCount;
        int   lapses;       // times reset to interval=1

        CardProgress() {
            interval = 1;
            easeFactor = 2.5f;
            dueDate = 0;    // due immediately
            reviewCount = 0;
            lapses = 0;
        }
    };

    // ── Card (combined) ───────────────────────────────────
    public ref struct Card {
        CardData^ data;
        CardProgress^ progress;

        Card(CardData^ d) {
            data = d;
            progress = gcnew CardProgress();
        }
    };

    // ── CardEngine ────────────────────────────────────────
    public ref class CardEngine {
    public:
        CardEngine() {
            cards = gcnew List<Card^>();
        }

        // Call these on startup
        void LoadCSV(String^ path);
        void LoadProgress(String^ path);
        void SaveProgress(String^ path);

        // Returns cards due today or overdue
        List<Card^>^ GetDueCards();

        // Returns all cards (for match mode etc)
        List<Card^>^ GetAllCards() { return cards; }

        // SM-2 update — call after every answer
        void UpdateProgress(Card^ card, Grade grade);

        // How many cards are due today
        int DueCount();

    private:
        List<Card^>^ cards;
        String^ progressPath;

        // CSV helpers
        String^ CleanCell(String^ cell);
        List<List<String^>^>^ ParseCSV(String^ raw);

        // Date helper — days since Jan 1 2000
        static int Today();
    };
} 