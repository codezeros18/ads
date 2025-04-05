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
    int likes;
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

    printf("\n=====================================\n");
    printf("           User Registration         \n");
    printf("=====================================\n");
    printf("Enter Username: ");
    scanf(" %99s", newUser->username);
    printf("Enter Password: ");
    scanf(" %99s", newUser->password);
    printf("Enter Email: ");
    scanf(" %99s", newUser->email);

    newUser->id = getLastUserID(filename) + 1;
    fprintf(file, "%d | %s | %s | %s\n", newUser->id, newUser->username, newUser->password, newUser->email);
    fclose(file);

    printf("\n✅User registered successfully! ID: %d\n", newUser->id);
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
    printf("\n=====================================\n");
    printf("              Login                  \n");
    printf("=====================================\n");
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
            printf("\n✅Login Successful! Welcome, %s (ID: %d)\n", userArray[userCount].username, userArray[userCount].id);
            fclose(file);
            return userArray[userCount].id;  // Return the logged-in user ID
        }
        userCount++;
    }

    fclose(file);
    printf("\n❌ Invalid login. Please try again.\n");
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
    printf("\n=====================================\n");
    printf("             Create Post             \n");
    printf("=====================================\n");
    printf(" Choose Post Type:\n");
    printf(" 1. Picture 📷\n");
    printf(" 2. Video 🎥\n");
    printf("=====================================\n");
    printf("> ");
    
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

    fprintf(file, "%d | %d | %s | %s | %s | %d\n", newPost->postId, newPost->userId, newPost->title, newPost->content, newPost->type, 0);
    fclose(file);

    printf("\n✅ Post created successfully! Post ID: %d\n", newPost->postId);
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
        printf("❌ No posts found!\n");
        return;
    }

    Post postArray[MAX_POSTS];
    int postCount = 0;

    printf("\n=====================================\n");
    printf("              Your Posts             \n");
    printf("=====================================\n");

    // Load all posts from file
    while (fscanf(file, "%d | %d | %99[^|] | %99[^|] | %99[^|] | %d\n", &postArray[postCount].postId, &postArray[postCount].userId,
                    postArray[postCount].title, postArray[postCount].content, postArray[postCount].type, &postArray[postCount].likes) != EOF) {
        postCount++;
    }

    fclose(file);

    // Sort posts by likes
    int choice;
    printf("Sort by:\n");
    printf("1. Newest\n");
    printf("2. Likes\n");
    printf("> ");
    scanf("%d", &choice);

    if (choice == 2) {
        // Sort posts by likes
        for (int i = 0; i < postCount - 1; i++) {
            for (int j = i + 1; j < postCount; j++) {
                if (postArray[i].likes < postArray[j].likes) {
                    // Swap posts
                    Post temp = postArray[i];
                    postArray[i] = postArray[j];
                    postArray[j] = temp;
                }
            }
        }
    }

    // Display sorted posts
    for (int i = 0; i < postCount; i++) {
        if (postArray[i].userId == userId) {
            printf(" 🆔 ID: %d\n", postArray[i].postId);
            printf(" 📌 Title: %s\n", postArray[i].title);
            printf(" 🎞️ Type: %s\n", postArray[i].type);
            printf(" ✏️ Content: %s\n", postArray[i].content);
            printf(" 👍 Likes: %d\n", postArray[i].likes);
            printf("-------------------------------------\n");
        }
    }
}

void deletePost(Post posts[], int *postCount, int userId) {
    int postId;
    printf("Enter the Post ID to delete: ");
    scanf("%d", &postId);

    int found = 0;
    for (int i = 0; i < *postCount; i++) {
        if (posts[i].postId == postId && posts[i].userId == userId) {
            // Shift all posts after i one step back
            for (int j = i; j < *postCount - 1; j++) {
                posts[j] = posts[j + 1];
            }
            (*postCount)--;
            found = 1;
            printf("🗑️ Post ID %d deleted successfully.\n", postId);
            break;
        }
    }

    if (!found) {
        printf("❌ Post not found or you don't have permission to delete it.\n");
    }
}

void likePost(Post posts[], int postCount) {
    int postId;
    printf("Enter the Post ID to like: ");
    scanf("%d", &postId);

    int found = 0;
    for (int i = 0; i < postCount; i++) {
        if (posts[i].postId == postId) {
            posts[i].likes++;
            printf("👍 Post ID %d liked! Total Likes: %d\n", postId, posts[i].likes);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("❌ Post not found.\n");
    }
}


void savePostsToFile(const char *filename, Post posts[], int postCount) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error writing to file!\n");
        return;
    }

    for (int i = 0; i < postCount; i++) {
        fprintf(file, "%d | %d | %s | %s | %s | %d\n",
                posts[i].postId, posts[i].userId, posts[i].title,
                posts[i].content, posts[i].type, posts[i].likes);
    }

    fclose(file);
}

int loadPostsFromFile(const char *filename, Post posts[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening posts file.\n");
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%d | %d | %99[^|] | %99[^|] | %99[^|] | %d\n",
                  &posts[count].postId,
                  &posts[count].userId,
                  posts[count].title,
                  posts[count].content,
                  posts[count].type,
                  &posts[count].likes) == 6) {
        count++;
    }

    fclose(file);
    return count;
}



void logout(int *userId) {
    printf("Logging out...\n");
    *userId = -1;  // Reset user ID to indicate no user is logged in
}


// Main function
int main() {
    int choice;
    int userId = -1;  // No user is logged in at the start

    Post posts[MAX_POSTS];  // Declare posts array
    int postCount = 0;      // Initialize post count

    while (1) {
        if (userId == -1) {
            printf("\n=====================================\n");
            printf("              Home Menu              \n");
            printf("=====================================\n");
            printf(" 1. Register\n");
            printf(" 2. Login\n");
            printf(" 3. Exit\n");
            printf("=====================================\n");
            printf("> ");
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
            printf("1. Create Post\n2. View Posts\n3. Like Post\n4. Delete Post\n5. Logout\nChoose an option: ");
            scanf("%d", &choice);
            
            // Flush input buffer
            while (getchar() != '\n');

            switch (choice) {
                case 1:
                    createPost("posts.txt", userId);
                    // Reload posts from file to memory
                    postCount = loadPostsFromFile("posts.txt", posts);
                    break;
                case 2:
                    viewPosts("posts.txt", userId);
                    break;
                case 3:
                    postCount = loadPostsFromFile("posts.txt", posts);  // Load latest
                    likePost(posts, postCount);
                    savePostsToFile("posts.txt", posts, postCount);     // Save updated
                    break;
                case 4:
                    postCount = loadPostsFromFile("posts.txt", posts);  // Load latest
                    deletePost(posts, &postCount, userId);
                    savePostsToFile("posts.txt", posts, postCount);     // Save updated
                    break;
                case 5:
                    logout(&userId);
                    break;
                default:
                    printf("Invalid choice! Try again.\n");
            }
        }
    }
}
