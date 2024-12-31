Create database CodeCarnival;
USE CodeCarnival;

CREATE TABLE Users (
    UserID INT IDENTITY(1,1) PRIMARY KEY,
    Username VARCHAR(50) UNIQUE NOT NULL,
    Email VARCHAR(100) UNIQUE NOT NULL,
    PasswordHash VARCHAR(255) NOT NULL,
    CreatedAt DATETIME DEFAULT GETDATE()
);

CREATE TABLE Games (
    GameID INT IDENTITY(1,1) PRIMARY KEY,
    GameName VARCHAR(100) UNIQUE NOT NULL,
    Description NVARCHAR(MAX),
    MaxLevels INT NOT NULL,
    MVPUserID INT NULL,
    FOREIGN KEY (MVPUserID) REFERENCES Users(UserID) ON DELETE SET NULL
);

CREATE TABLE Levels (
    LevelID INT IDENTITY(1,1) PRIMARY KEY,
    GameID INT NOT NULL,
    LevelNumber INT NOT NULL,
    LevelDescription NVARCHAR(MAX),
    FOREIGN KEY (GameID) REFERENCES Games(GameID) ON DELETE CASCADE
);

CREATE TABLE Scores (
    ScoreID INT IDENTITY(1,1) PRIMARY KEY,
    UserID INT NOT NULL,
    GameID INT NOT NULL,
    Score INT NOT NULL,
    TimeSpent INT DEFAULT 0, -- in seconds
    AchievedAt DATETIME DEFAULT GETDATE(),
    FOREIGN KEY (UserID) REFERENCES Users(UserID) ON DELETE CASCADE,
    FOREIGN KEY (GameID) REFERENCES Games(GameID) ON DELETE CASCADE
);

CREATE TABLE UserGameProgress (
    ProgressID INT IDENTITY(1,1) PRIMARY KEY,
    UserID INT NOT NULL,
    GameID INT NOT NULL,
    CurrentLevel INT NOT NULL,
    LastPlayed DATETIME DEFAULT GETDATE(),
    FOREIGN KEY (UserID) REFERENCES Users(UserID) ON DELETE CASCADE,
    FOREIGN KEY (GameID) REFERENCES Games(GameID) ON DELETE CASCADE
);

CREATE TABLE Rewards (
    RewardID INT IDENTITY(1,1) PRIMARY KEY,
    UserID INT NOT NULL,
    GameID INT NOT NULL,
    RewardName VARCHAR(100) NOT NULL,
    AchievedAt DATETIME DEFAULT GETDATE(),
    FOREIGN KEY (UserID) REFERENCES Users(UserID) ON DELETE CASCADE,
    FOREIGN KEY (GameID) REFERENCES Games(GameID) ON DELETE CASCADE
);

CREATE TABLE GameLogs (
    LogID INT IDENTITY(1,1) PRIMARY KEY,
    UserID INT NOT NULL,
    GameID INT NOT NULL,
    LogData NVARCHAR(MAX), -- JSON or serialized data for flexibility
    LoggedAt DATETIME DEFAULT GETDATE(),
    FOREIGN KEY (UserID) REFERENCES Users(UserID) ON DELETE CASCADE,
    FOREIGN KEY (GameID) REFERENCES Games(GameID) ON DELETE CASCADE
);

--------------------------------------------------------------------------------------------

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER PROCEDURE [dbo].[updateGameProgress]
    @userID INT,
    @gameID INT,
    @currentLevel INT
AS
BEGIN
    -- Check if an entry already exists for the given UserID and GameID
    IF EXISTS (
        SELECT 1 
        FROM UserGameProgress
        WHERE UserID = @userID AND GameID = @gameID
    )
    BEGIN
        -- Update the existing record
        UPDATE UserGameProgress
        SET CurrentLevel = @currentLevel,
            LastPlayed = GETDATE()
        WHERE UserID = @userID AND GameID = @gameID;
    END
    ELSE
    BEGIN
        -- Insert a new record
        INSERT INTO UserGameProgress (UserID, GameID, CurrentLevel, LastPlayed)
        VALUES (@userID, @gameID, @currentLevel, GETDATE());
    END
END;
GO
---------------------------------------------------------------------------------------------
CREATE PROCEDURE [dbo].[updateMVP]
    @gameID INT
AS
BEGIN
    SET NOCOUNT ON;

    BEGIN TRY
        BEGIN TRANSACTION;

        -- Find the user with the highest cumulative score for the given game
        DECLARE @mvpUserID INT;

        SELECT TOP 1 
            @mvpUserID = UserID
        FROM Scores
        WHERE GameID = @gameID
        GROUP BY UserID
        ORDER BY SUM(Score) DESC;

        -- Update the MVPUserID in the Games table
        UPDATE Games
        SET MVPUserID = @mvpUserID
        WHERE GameID = @gameID;

        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO
----------------------------------------------------------------------------------------------
CREATE TRIGGER trg_updateMVP
ON Scores
AFTER INSERT, UPDATE
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @gameID INT;

    -- Get the GameID from the inserted or updated row
    SELECT @gameID = GameID FROM inserted;

    -- Call the stored procedure to update the MVP
    EXEC updateMVP @gameID;
END;
GO
----------------------------------------------------------------------------------------------
CREATE PROCEDURE [dbo].[updateGameState]
    @userID INT,
    @gameID INT,
    @currentLevel INT,
    @score INT = NULL,           -- Optional score
    @timeSpent INT = NULL,       -- Optional time spent in seconds
    @logData NVARCHAR(MAX) = NULL -- Optional log data (JSON or serialized format)
AS
BEGIN
    SET NOCOUNT ON;

    BEGIN TRY
        BEGIN TRANSACTION;

        -- 1. Update or Insert UserGameProgress
        IF EXISTS (
            SELECT 1 
            FROM UserGameProgress
            WHERE UserID = @userID AND GameID = @gameID
        )
        BEGIN
            -- Update existing progress
            UPDATE UserGameProgress
            SET CurrentLevel = @currentLevel,
                LastPlayed = GETDATE()
            WHERE UserID = @userID AND GameID = @gameID;
        END
        ELSE
        BEGIN
            -- Insert new progress
            INSERT INTO UserGameProgress (UserID, GameID, CurrentLevel, LastPlayed)
            VALUES (@userID, @gameID, @currentLevel, GETDATE());
        END

        -- 2. Insert Score
        IF @score IS NOT NULL
        BEGIN
            INSERT INTO Scores (UserID, GameID, Score, TimeSpent, AchievedAt)
            VALUES (@userID, @gameID, @score, @timeSpent, GETDATE());
        END

        -- 3. Insert Log
        IF @logData IS NOT NULL
        BEGIN
            INSERT INTO GameLogs (UserID, GameID, LogData, LoggedAt)
            VALUES (@userID, @gameID, @logData, GETDATE());
        END

        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        ROLLBACK TRANSACTION;
        THROW; -- Re-throw the error for debugging
    END CATCH
END;
GO
---------------------------------------------------------------------------------------------