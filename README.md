### Code Carnival 🎮

**Code Carnival** is an interactive gaming platform that showcases practical implementation of **data structures**, **algorithms**, and **database management**. This project is designed to provide an engaging learning experience by integrating theoretical concepts into fun and challenging games.

---

### 📂 Database Overview
The database is the core of Code Carnival, tracking user data, game progress, scores, levels, and rewards. It includes:

1. **Users Table**:
   - Stores user credentials with secure password hashing.
   - Tracks account creation timestamps.

2. **Games Table**:
   - Maintains game details like name, description, maximum levels, and the MVP (Most Valuable Player).

3. **Levels Table**:
   - Manages individual levels of each game, including descriptions.

4. **Scores Table**:
   - Tracks scores, time spent, and achievement timestamps for each user in every game.

5. **UserGameProgress Table**:
   - Monitors user progress by recording the current level and last played time.

6. **Rewards Table**:
   - Records rewards earned by users in specific games.

7. **GameLogs Table**:
   - Logs gameplay data in JSON or serialized formats for detailed activity tracking.

---

### 🕹️ Games and Concepts

#### 1. **Snake**
   - **Data Structures**:
     - **Queue**: For snake body management (FIFO behavior).
     - **Enum**: For direction control (UP, DOWN, LEFT, RIGHT).
     - **Linked List**: To dynamically grow the snake.
   - **Database**:
     - Tracks user progress, scores, and rewards for achieving milestones.

#### 2. **Chess** *(Planned)*
   - **Data Structures**:
     - **Trees**: For move prediction and AI decision-making.
     - **2D Arrays**: For board representation.
   - **Algorithms**:
     - Minimax Algorithm with Alpha-Beta Pruning for AI.
   - **Database**:
     - Logs match outcomes, user wins/losses, and MVP tracking.

#### 3. **Tic Tac Toe**
   - **Data Structures**:
     - **Trees**: For move prediction and game outcome evaluation.
     - **2D Arrays**: For board representation.
   - **Database**:
     - Stores match outcomes, user progress, and rewards.

#### 4. **Battle Ship**
   - **Data Structures**:
     - **2D Arrays**: For board layout and ship placement.
     - **Stacks**: For undo functionality and move history.
   - **Database**:
     - Logs gameplay data and user achievements.

#### 5. **Maze Runner**
   - **Data Structures**:
     - **2D Dynamic Arrays**: For the maze layout.
     - **Stack**: For backtracking during DFS.
     - **Queue**: For BFS traversal.
     - **Priority Queue**: For Best First Search.
     - **Maps**: For storing visited nodes.
   - **Algorithms**:
     - **Depth First Search (DFS)**: Explores paths recursively.
     - **Breadth First Search (BFS)**: Finds the shortest path in an unweighted graph.
     - **Best First Search**: Combines heuristics with graph traversal.
     - **Dijkstra's Algorithm**: Finds the shortest path in a weighted graph.
   - **Database**:
     - Tracks progress, scores, and gameplay logs.

---

### 🔧 Key Algorithms
1. **Minimax Algorithm** (Chess):
   - Decision-making algorithm for AI in a zero-sum game.
   - Evaluates moves and predicts opponent responses.

2. **Dijkstra's Algorithm** (Maze Runner):
   - Finds the shortest path in weighted graphs.
   - Optimizes navigation in complex mazes.

3. **DFS and BFS**:
   - DFS: Backtracking for path exploration.
   - BFS: Level-by-level exploration for unweighted pathfinding.

4. **Best First Search**:
   - Combines BFS with heuristics for optimized exploration.

---

### 🚀 Features
- **User Authentication**:
  - Secure login with password encryption.
- **Game Tracking**:
  - Monitors user scores, progress, and achievements.
- **Reward System**:
  - Recognizes and logs player milestones.
- **Dynamic Gameplay Management**:
  - Real-time progress updates with efficient algorithms.

---

### 📂 Stored Procedures and Triggers
- **updateGameProgress**:
  - Updates or inserts user progress dynamically.
- **updateMVP**:
  - Automatically updates the MVP of a game based on cumulative scores.
- **updateGameState**:
  - Manages user progress, scores, and logs in a single transaction.
- **Trigger: trg_updateMVP**:
  - Automates MVP updates when scores are inserted or updated.

---

### 🎯 Summary
**Code Carnival** is a robust gaming platform that integrates secure database management, advanced algorithms, and efficient data structures. It serves as both a learning tool and an engaging project for developers and gamers alike. Explore the repository and contribute to its development! 🎉
