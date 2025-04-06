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
    char type[MAX_STR]; 
    int likes;
} Post;

typedef struct {
    int commentId;
    int postId;
    int commenterId;
    char text[256];
} Comment;

#define MAX_COMMENTS 100

int getLastCommentID(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int lastID = 0;
    while (fscanf(file, "%d|%*d|%*d|%*[^\n]\n", &lastID) != EOF);
    fclose(file);
    return lastID;
}

void addCommentToFile(const char *filename, int postId, int commenterId) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error opening comment file!\n");
        return;
    }

    Comment newComment;
    newComment.commentId = getLastCommentID(filename) + 1;
    newComment.postId = postId;
    newComment.commenterId = commenterId;

    printf("Enter your comment: ");
    fgets(newComment.text, sizeof(newComment.text), stdin);
    newComment.text[strcspn(newComment.text, "\n")] = '\0';

    fprintf(file, "%d|%d|%d|%s\n", newComment.commentId, newComment.postId, newComment.commenterId, newComment.text);
    fclose(file);

    printf("\n✅ Comment added successfully!\n");
}

void viewCommentsForPost(const char *filename, int postId) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return;
    }

    Comment c;
    printf("\n📌 Comments for Post ID %d:\n", postId);
    int found = 0;
    while (fscanf(file, "%d|%d|%d|%255[^\n]\n", &c.commentId, &c.postId, &c.commenterId, c.text) == 4) {
        if (c.postId == postId) {
            printf("   - [User %d]: %s\n", c.commenterId, c.text);
            found = 1;
        }
    }
    if (!found) {
        printf("   No comments yet.\n");
    }

    fclose(file);
}

void viewPostsWithComments(const char *postFilename, const char *commentFilename, int userId) {
    FILE *file = fopen(postFilename, "r");
    if (!file) {
        printf("❌ No posts found!\n");
        return;
    }

    Post post;
    printf("\n=====================================\n");
    printf("          Your Posts & Comments       \n");
    printf("=====================================\n");
    while (fscanf(file, "%d|%d|%99[^|]|%99[^|]|%99[^|]|%d\n", &post.postId, &post.userId,
                  post.title, post.content, post.type, &post.likes) == 6) {
        if (post.userId == userId) {
            printf(" 🆔 ID: %d\n 📌 Title: %s\n 🎞️ Type: %s\n ✏️ Content: %s\n ❤️ Likes: %d\n",
                   post.postId, post.title, post.type, post.content, post.likes);
            viewCommentsForPost(commentFilename, post.postId);
            printf("-------------------------------------\n");
        }
    }
    fclose(file);
}

void deleteCommentsByPostId(const char *filename, int postIdToDelete) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    FILE *temp = fopen("temp_comments.txt", "w");
    if (!temp) {
        fclose(file);
        return;
    }

    Comment c;
    while (fscanf(file, "%d|%d|%d|%255[^\n]\n", &c.commentId, &c.postId, &c.commenterId, c.text) == 4) {
        if (c.postId != postIdToDelete) {
            fprintf(temp, "%d|%d|%d|%s\n", c.commentId, c.postId, c.commenterId, c.text);
        }
    }

    fclose(file);
    fclose(temp);
    remove(filename);
    rename("temp_comments.txt", filename);
}

// Function to get the last User ID from file
int getLastUserID(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int id, maxId = 0;
    char dummy1[100], dummy2[100], dummy3[100];

    while (fscanf(file, "%d|%99[^|]|%99[^|]|%99[^\n]\n", &id, dummy1, dummy2, dummy3) == 4) {
        if (id > maxId) maxId = id;
    }

    fclose(file);
    return maxId;
}

int getLastPostID(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int lastID = 0;
    while (fscanf(file, "%d|%*d|%*s|%*s|%*s\n", &lastID) != EOF);
    fclose(file);
    return lastID;
}

// Function to register a new user
void registerUser(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Couldn't read users, creating a new file...\n");
    }

    char lines[100][200]; 
    int count = 0;

    while (file && fgets(lines[count], sizeof(lines[count]), file)) {
        count++;
    }
    if (file) fclose(file);

    User *newUser = (User *)malloc(sizeof(User));
    if (!newUser) {
        printf("Memory allocation failed!\n");
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

    file = fopen(filename, "w");
    if (!file) {
        printf("Error writing file!\n");
        free(newUser);
        return;
    }

    fprintf(file, "%d|%s|%s|%s\n", newUser->id, newUser->username, newUser->password, newUser->email);

    for (int i = 0; i < count; i++) {
        fputs(lines[i], file);
    }

    fclose(file);
    printf("\n✅ User registered successfully! ID: %d\n", newUser->id);
    free(newUser);
}

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
    
    while (getchar() != '\n');  

    User userArray[MAX_USERS];
    int userCount = 0;

    while (fscanf(file, "%d|%99[^|]|%99[^|]|%99[^\n]\n",
                &userArray[userCount].id,
                userArray[userCount].username,
                userArray[userCount].password,
                userArray[userCount].email) == 4) {
        if (strcmp(userArray[userCount].username, username) == 0 &&
            strcmp(userArray[userCount].password, password) == 0) {
            printf("\n✅ Login Successful! Welcome, %s (ID: %d)\n", userArray[userCount].username, userArray[userCount].id);
            fclose(file);
            return userArray[userCount].id;
            }
        userCount++;
    }


    fclose(file);
    printf("\n❌ Invalid login. Please try again.\n");
    return -1;
}

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
    newPost->title[strcspn(newPost->title, "\n")] = '\0';

    printf("Enter Post Content: ");
    fgets(newPost->content, sizeof(newPost->content), stdin);
    newPost->content[strcspn(newPost->content, "\n")] = '\0';

    // Append file extension
    strcat(newPost->content, (typeChoice == 1) ? ".png" : ".mp4");

    fprintf(file, "%d|%d|%s|%s|%s|%d\n", newPost->postId, newPost->userId, newPost->title, newPost->content, newPost->type, 0);
    fclose(file);

    printf("\n✅ Post created successfully! Post ID: %d\n", newPost->postId);
    free(newPost);
}

void viewPosts(const char *postFilename, const char *commentFilename, int userId) {
    if (userId == -1) {
        printf("You must be logged in to view posts.\n");
        return;
    }

    FILE *file = fopen(postFilename, "r");
    if (!file) {
        printf("❌ No posts found!\n");
        return;
    }

    Post post;
    printf("\n=====================================\n");
    printf("              Your Posts             \n");
    printf("=====================================\n");
    while (fscanf(file, "%d|%d|%99[^|]|%99[^|]|%99[^\n]\n",
                  &post.postId, &post.userId, post.title, post.content, post.type) == 5) {
        if (post.userId == userId) {
            printf(" 🆔 ID: %d\n", post.postId);
            printf(" 📌 Title: %s\n", post.title);
            printf(" 🎞️ Type: %s\n", post.type);
            printf(" ✏️ Content: %s\n", post.content);

            viewCommentsForPost(commentFilename, post.postId);

            printf("-------------------------------------\n");
        }
    }

    fclose(file);
}


void deletePost(Post posts[], int *postCount, int userId, const char *commentFilename) {
    int postId;
    printf("Enter the Post ID to delete: ");
    scanf("%d", &postId);
    getchar(); 

    int found = 0;
    for (int i = 0; i < *postCount; i++) {
        if (posts[i].postId == postId && posts[i].userId == userId) {
            for (int j = i; j < *postCount - 1; j++) {
                posts[j] = posts[j + 1];
            }
            (*postCount)--;
            deleteCommentsByPostId(commentFilename, postId);
            found = 1;
            printf("🗑️ Post ID %d and its comments deleted successfully.\n", postId);
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
        fprintf(file, "%d|%d|%s|%s|%s|%d\n",
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
    while (fscanf(file, "%d|%d|%99[^|]|%99[^|]|%99[^|]|%d\n",
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
    *userId = -1; 
}

// Main function
int main() {
    int choice;
    int userId = -1; 

    Post posts[MAX_POSTS]; 
    int postCount = 0;      

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
            printf("1. Create Post\n");
            printf("2. View Posts\n");
            printf("3. Like Post\n");
            printf("4. Delete Post\n");
            printf("5. Comment on Post\n");
            printf("6. Logout\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            
            while (getchar() != '\n');

            switch (choice) {
                case 1:
                    createPost("posts.txt", userId);
                    postCount = loadPostsFromFile("posts.txt", posts);
                    break;
                case 2:
                    viewPostsWithComments("posts.txt", "comments.txt", userId);
                    break;
                case 3:
                    postCount = loadPostsFromFile("posts.txt", posts);
                    likePost(posts, postCount);
                    savePostsToFile("posts.txt", posts, postCount);
                    break;
                case 4:
                    postCount = loadPostsFromFile("posts.txt", posts);
                    deletePost(posts, &postCount, userId, "comments.txt"); 
                    savePostsToFile("posts.txt", posts, postCount);
                    break;
                case 5:
                    printf("Enter Post ID to comment on: ");
                    int targetPostId;
                    scanf("%d", &targetPostId);
                    getchar(); 
                    addCommentToFile("comments.txt", targetPostId, userId);
                    break;
                case 6:
                    userId = -1;
                    break;
                default:
                    printf("Invalid choice! Try again.\n");
            }
        }
    }
}
