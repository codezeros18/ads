#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_POSTS 100
#define MAX_STR 100

typedef struct {
    int id;
    char username[MAX_STR];
    char password[MAX_STR];
    char email[MAX_STR];
} User;

typedef struct {
    int postId;
    int userId;
    char title[MAX_STR];
    char content[MAX_STR];
    char type[MAX_STR];  // "Picture" or "Video"
} Post;

// Function to get the last User ID from file
int getLastUserID(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int lastID = 0;
    while (fscanf(file, "%d | %*s | %*s | %*s\n", &lastID) != EOF);
    fclose(file);
    return lastID;
}

// Function to get the last Post ID from file
int getLastPostID(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int lastID = 0;
    while (fscanf(file, "%d | %*d | %*s | %*s | %*s\n", &lastID) != EOF);
    fclose(file);
    return lastID;
}

// Function to register a new user
void registerUser(const char *filename) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    User *newUser = (User *)malloc(sizeof(User));
    if (!newUser) {
        printf("Memory allocation failed!\n");
        fclose(file);
        return;
    }

    printf("Enter Username: ");
    scanf(" %99s", newUser->username);
    printf("Enter Password: ");
    scanf(" %99s", newUser->password);
    printf("Enter Email: ");
    scanf(" %99s", newUser->email);

    newUser->id = getLastUserID(filename) + 1;
    fprintf(file, "%d | %s | %s | %s\n", newUser->id, newUser->username, newUser->password, newUser->email);
    fclose(file);

    printf("User registered successfully! ID: %d\n", newUser->id);
    free(newUser); // Free allocated memory
}

// Function to login a user
int loginUser(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("No users found!\n");
        return -1;
    }

    char username[MAX_STR], password[MAX_STR];
    printf("Enter Username: ");
    scanf(" %99s", username);
    printf("Enter Password: ");
    scanf(" %99s", password);
    
    // Flush input buffer
    while (getchar() != '\n');  

    User userArray[MAX_USERS];
    int userCount = 0;

    while (fscanf(file, "%d | %99s | %99s | %99s\n", &userArray[userCount].id, userArray[userCount].username,
                  userArray[userCount].password, userArray[userCount].email) != EOF) {
        if (strcmp(userArray[userCount].username, username) == 0 &&
            strcmp(userArray[userCount].password, password) == 0) {
            printf("Login Successful! Welcome, %s (ID: %d)\n", userArray[userCount].username, userArray[userCount].id);
            fclose(file);
            return userArray[userCount].id;  // Return the logged-in user ID
        }
        userCount++;
    }

    fclose(file);
    printf("Invalid username or password!\n");
    return -1;
}


// Function to create a new post
// Function to create a new post

void createPost(const char *filename, int userId) {
    if (userId == -1) {
        printf("You must be logged in to create a post.\n");
        return;
    }

    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    Post *newPost = (Post *)malloc(sizeof(Post));
    if (!newPost) {
        printf("Memory allocation failed!\n");
        fclose(file);
        return;
    }

    newPost->postId = getLastPostID(filename) + 1;
    newPost->userId = userId;

    int typeChoice;
    printf("Choose Post Type:\n1. Picture\n2. Video\nEnter choice: ");
    
    if (scanf("%d", &typeChoice) != 1) {
        printf("Invalid input!\n");
        free(newPost);
        fclose(file);
        return;
    }

    // **Flush input buffer to prevent fgets issues**
    while (getchar() != '\n'); 

    if (typeChoice == 1) {
        strcpy(newPost->type, "Picture");
    } else if (typeChoice == 2) {
        strcpy(newPost->type, "Video");
    } else {
        printf("Invalid choice!\n");
        free(newPost);
        fclose(file);
        return;
    }

    printf("Enter Post Title: ");
    fgets(newPost->title, sizeof(newPost->title), stdin);
    newPost->title[strcspn(newPost->title, "\n")] = '\0';  // Remove newline

    printf("Enter Post Content: ");
    fgets(newPost->content, sizeof(newPost->content), stdin);
    newPost->content[strcspn(newPost->content, "\n")] = '\0';  // Remove newline

    // Append file extension
    strcat(newPost->content, (typeChoice == 1) ? ".png" : ".mp4");

    fprintf(file, "%d | %d | %s | %s | %s\n", newPost->postId, newPost->userId, newPost->title, newPost->content, newPost->type);
    fclose(file);

    printf("Post created successfully! Post ID: %d\n", newPost->postId);
    free(newPost);
}


// Function to view all posts of the logged-in user
void viewPosts(const char *filename, int userId) {
    if (userId == -1) {
        printf("You must be logged in to view posts.\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("No posts found!\n");
        return;
    }

    Post postArray[MAX_POSTS];
    int postCount = 0;

    printf("\n=== Your Posts ===\n");
    while (fscanf(file, "%d | %d | %99[^|] | %99[^|] | %99[^\n]\n", &postArray[postCount].postId, &postArray[postCount].userId,
                  postArray[postCount].title, postArray[postCount].content, postArray[postCount].type) != EOF) {
        if (postArray[postCount].userId == userId) {
            printf("Post ID: %d\nTitle: %s\nContent: %s\nType: %s\n\n",
                   postArray[postCount].postId, postArray[postCount].title,
                   postArray[postCount].content, postArray[postCount].type);
        }
        postCount++;
    }

    fclose(file);
}

void logout(int *userId) {
    printf("Logging out...\n");
    *userId = -1;  // Reset user ID to indicate no user is logged in
}


// Main function
int main() {
    int choice;
    int userId = -1;  // No user is logged in at the start

    while (1) {
        if (userId == -1) {
            printf("\n=== Home ===\n");
            printf("1. Register\n2. Login\n3. Exit\nChoose an option: ");
            scanf("%d", &choice);
            
            // Flush input buffer
            while (getchar() != '\n');

            switch (choice) {
                case 1:
                    registerUser("users.txt");
                    break;
                case 2:
                    userId = loginUser("users.txt");
                    break;
                case 3:
                    printf("Exiting...\n");
                    return 0;
                default:
                    printf("Invalid choice! Try again.\n");
            }
        } else {
            printf("\n=== User Menu ===\n");
            printf("1. Create Post\n2. View Posts\n3. Log Out\nChoose an option: ");
            scanf("%d", &choice);
            
            // Flush input buffer
            while (getchar() != '\n');

            switch (choice) {
                case 1:
                    createPost("posts.txt", userId);
                    break;
                case 2:
                    viewPosts("posts.txt", userId);
                    break;
                case 3:
                    logout(&userId);
                    break;
                default:
                    printf("Invalid choice! Try again.\n");
            }
        }
    }
}
