#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR 100
#define USER_FILE "users.txt"
#define POST_FILE "posts.txt"

typedef struct User {
    int id;
    char username[MAX_STR], password[MAX_STR], email[MAX_STR];
    struct User *next;
} User;

typedef struct Post {
    int id, userId, likes;
    char title[MAX_STR], content[MAX_STR], type[MAX_STR];
    struct Post *next;
} Post;

typedef struct Comment {
    int commentId; 
    int postId;
    int commenterId;
    char text[256];
    struct Comment *next;
} Comment;

User *loadUsers(), *registerUser(User *head), *loginUser(User *head, int *userId);
Post *loadPosts(), *createPost(Post *head, int userId);
void saveUsers(User *head), savePosts(Post *head);
void viewPosts(Post *head, int userId, Comment *comments); 
void freeUsers(User *head), freePosts(Post *head);
Post *deletePost(Post *head, int postId); 
void likePost(Post *head, int postId);  
Comment *addComment(Comment *head, int postId, int commenterId, const char *text, const char *filename);
void deleteCommentsByPostId(const char *filename, int postId);
bool postExists(const char *filename, int postId);

Comment* loadComments() {
    FILE *fp = fopen("comments.txt", "r");
    if (!fp) return NULL;

    Comment *head = NULL, *tail = NULL;
    Comment temp;
    while (fscanf(fp, "%d|%d|%d|%255[^\n]\n", &temp.commentId, &temp.postId, &temp.commenterId, temp.text) == 4) {
        Comment *newComment = (Comment*)malloc(sizeof(Comment));
        *newComment = temp;
        newComment->next = NULL;

        if (!head) head = tail = newComment;
        else {
            tail->next = newComment;
            tail = newComment;
        }
    }

    fclose(fp);
    return head;
}


void saveComments(Comment *head) {
    FILE *file = fopen("comments.txt", "w");
    if (!file) {
        printf("❌ Failed to open comments.txt for writing\n");
        return;
    }

    Comment *curr = head;
    int commentId = 1;
    while (curr) {
        fprintf(file, "%d|%d|%d|%s\n", commentId++, curr->postId, curr->commenterId, curr->text);
        curr = curr->next;
    }

    fclose(file);
}


Comment *addComment(Comment *head, int postId, int commenterId, const char *text, const char *filename) {
    if (!postExists(filename, postId)) {
        printf("❌ Post ID %d does not exist. Comment not added.\n", postId);
        return NULL;
    }

    Comment *newNode = malloc(sizeof(Comment));
    newNode->postId = postId;
    newNode->commenterId = commenterId;
    strcpy(newNode->text, text);
    newNode->next = head;
    return newNode;
}

bool postExists(const char *filename, int postId) {
    FILE *file = fopen(filename, "r");
    if (!file) return false;

    Post p;
    while (fscanf(file, "%d|%d|%[^|]|%[^|]|%[^\n]\n", &p.id, &p.userId, p.title, p.type, p.content) == 5) {
        if (p.id == postId) {
            fclose(file);
            return true;
        }        
    }
    fclose(file);
    return false;
}

int main() {
    User *users = loadUsers();
    Post *posts = loadPosts();
    Comment *comments = loadComments();

    int userId = -1, choice, postId;

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
            scanf("%d", &choice); getchar();

            switch (choice) {
                case 1:
                    users = registerUser(users);
                    saveUsers(users);
                    break;
                case 2:
                    users = loginUser(users, &userId);
                    break;
                case 3:
                    saveUsers(users);
                    savePosts(posts);
                    saveComments(comments); 
                    freeUsers(users);
                    freePosts(posts);

                    while (comments) {
                        Comment *temp = comments;
                        comments = comments->next;
                        free(temp);
                    }
                    return 0;
                default:
                    printf("❌ Invalid option. Try again.\n");
            }
        } else {
            printf("\n=== User Menu ===\n");
            printf("1. Create Post\n2. View Posts\n3. Like Post\n4. Delete Post\n5. Comment on Post\n6. Logout\n> ");
            scanf("%d", &choice); getchar();

            switch (choice) {
                case 1:
                    posts = createPost(posts, userId);
                    savePosts(posts);
                    break;
                case 2:
                    viewPosts(posts, userId, comments);
                    break;
                case 3:
                    printf("Enter Post ID to like: ");
                    scanf("%d", &postId); getchar();
                    likePost(posts, postId);
                    savePosts(posts);
                    break;
                case 4:
                    printf("Enter Post ID to delete: ");
                    scanf("%d", &postId); getchar();
                    posts = deletePost(posts, postId);
                    savePosts(posts);
                    break;
                    case 5:
                    printf("Enter Post ID to comment on: ");
                    scanf("%d", &postId); getchar();
                
                    char text[256];
                    printf("Enter your comment: ");
                    fgets(text, sizeof(text), stdin);
                    text[strcspn(text, "\n")] = 0; // remove newline
                
                    comments = addComment(comments, postId, userId, text, "posts.txt");
                    if (comments)
                        saveComments(comments);
                    else
                        printf("❌ Cannot comment — Post ID %d does not exist.\n", postId);
                    break;                
                case 6:
                    userId = -1;
                    printf("👋 Logged out!\n");
                    break;
                default:
                    printf("❌ Invalid option. Try again.\n");
            }
        }
    }
}

Post *deletePost(Post *head, int postId) {
    Post *prev = NULL, *cur = head;

    while (cur) {
        if (cur->id == postId) {
            if (prev) prev->next = cur->next;
            else head = cur->next;

            free(cur);

            deleteCommentsByPostId("comments.txt", postId);

            printf("\n✅ Post ID %d and its comments deleted successfully!\n", postId);
            return head;
        }
        prev = cur;
        cur = cur->next;
    }

    printf("\n❌ Post ID %d not found!\n", postId);
    return head;
}

void deleteCommentsByPostId(const char *filename, int postId) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("⚠️ Could not open %s\n", filename);
        return;
    }

    FILE *temp = fopen("temp_comments.txt", "w");
    if (!temp) {
        fclose(file);
        printf("⚠️ Could not create temp file.\n");
        return;
    }

    Comment c;

    while (fscanf(file, "%d|%d|%99[^\n]\n", &c.postId, &c.commenterId, c.text) == 3) {
        if (c.postId != postId) {
            fprintf(temp, "%d|%d|%s\n", c.postId, c.commenterId, c.text);
        }
    }


    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_comments.txt", filename);
}

void likePost(Post *head, int postId) {
    Post *cur = head;

    while (cur) {
        if (cur->id == postId) {
            cur->likes++;
            printf("\n✅ Post ID %d liked! Total Likes: %d\n", postId, cur->likes);
            return;
        }
        cur = cur->next;
    }

    printf("\n❌ Post ID %d not found!\n", postId);
}

User *registerUser(User *head) {
    User *newUser = malloc(sizeof(User));
    if (!newUser) return head;

    printf("\n=====================================\n");
    printf("           User Registration         \n");
    printf("=====================================\n");
    printf(" Enter Username: "); fgets(newUser->username, MAX_STR, stdin);
    printf(" Enter Password: "); fgets(newUser->password, MAX_STR, stdin);
    printf(" Enter Email: "); fgets(newUser->email, MAX_STR, stdin);

    newUser->username[strcspn(newUser->username, "\n")] = '\0';
    newUser->password[strcspn(newUser->password, "\n")] = '\0';
    newUser->email[strcspn(newUser->email, "\n")] = '\0';

    newUser->id = (head == NULL) ? 1 : head->id + 1;
    newUser->next = head;

    printf("\n✅ User registered successfully! ID: %d\n", newUser->id);
    return newUser;
}

User *loginUser(User *head, int *userId) {
    char username[MAX_STR], password[MAX_STR];

    printf("\n=====================================\n");
    printf("              Login                  \n");
    printf("=====================================\n");
    printf(" Enter Username: "); fgets(username, MAX_STR, stdin);
    printf(" Enter Password: "); fgets(password, MAX_STR, stdin);

    username[strcspn(username, "\n")] = '\0';
    password[strcspn(password, "\n")] = '\0';

    for (User *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->username, username) == 0 && strcmp(cur->password, password) == 0) {
            *userId = cur->id;
            printf("\n✅ Login Successful! Welcome, %s (ID: %d)\n", username, *userId);
            return head;
        }
    }
    printf("\n❌ Invalid login. Please try again.\n");
    return head;
}

Post *createPost(Post *head, int userId) {
    Post *newPost = malloc(sizeof(Post));
    if (!newPost) return head;

    newPost->userId = userId;
    newPost->likes = 0;

    printf("\n=====================================\n");
    printf("             Create Post             \n");
    printf("=====================================\n");
    printf(" Choose Post Type:\n");
    printf(" 1. Picture 📷\n");
    printf(" 2. Video 🎥\n");
    printf("=====================================\n");
    printf("> ");
    
    int choice;
    scanf("%d", &choice); getchar();
    strcpy(newPost->type, (choice == 1) ? "Picture" : "Video");

    printf("\n Enter Post Title: "); fgets(newPost->title, MAX_STR, stdin);
    printf(" Enter Post Content: "); fgets(newPost->content, MAX_STR, stdin);

    newPost->title[strcspn(newPost->title, "\n")] = '\0';
    newPost->content[strcspn(newPost->content, "\n")] = '\0';

    strcat(newPost->content, (choice == 1) ? ".png" : ".mp4");
    newPost->id = (head == NULL) ? 1 : head->id + 1;
    newPost->next = head;

    printf("\n✅ Post created successfully! Post ID: %d\n", newPost->id);
    return newPost;
}

void viewPosts(Post *head, int userId, Comment *comments) {
    printf("\n=====================================\n");
    printf("              Your Posts             \n");
    printf("=====================================\n");

    int found = 0;
    for (Post *cur = head; cur; cur = cur->next) {
        if (cur->userId == userId) {
            printf(" 🆔 ID: %d\n", cur->id);
            printf(" 📌 Title: %s\n", cur->title);
            printf(" 👍 Likes: %d\n", cur->likes);
            printf(" 🎞️ Type: %s\n", cur->type);
            printf(" ✏️ Content: %s\n", cur->content);
            
            printf(" 💬 Comments:\n");
            int commentFound = 0;
            for (Comment *c = comments; c != NULL; c = c->next) {
                if (c->postId == cur->id) {
                    printf("   - User %d: %s\n", c->commenterId, c->text);
                    commentFound = 1;
                }
            }
            if (!commentFound) {
                printf("   No comments yet.\n");
            }

            printf("-------------------------------------\n");
            found = 1;
        }
    }

    if (!found) {
        printf("❌ No posts found!\n");
    }
}

void saveUsers(User *head) {
    FILE *file = fopen(USER_FILE, "w");
    for (User *cur = head; cur; cur = cur->next)
        fprintf(file, "%d|%s|%s|%s\n", cur->id, cur->username, cur->password, cur->email);
    fclose(file);
}

User *loadUsers() {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return NULL;
    User *head = NULL;
    while (!feof(file)) {
        User *newUser = malloc(sizeof(User));
        if (fscanf(file, "%d|%[^|]|%[^|]|%[^\n]\n", &newUser->id, newUser->username, newUser->password, newUser->email) != 4) {
            free(newUser); break;
        }
        newUser->next = head;
        head = newUser;
    }
    fclose(file);
    return head;
}

void savePosts(Post *head) {
    FILE *file = fopen(POST_FILE, "w");
    for (Post *cur = head; cur; cur = cur->next)
        fprintf(file, "%d|%d|%s|%s|%s|%d\n", cur->id, cur->userId, cur->title, cur->content, cur->type, cur->likes);
    fclose(file);
}

Post *loadPosts() {
    FILE *file = fopen(POST_FILE, "r");
    if (!file) return NULL;
    Post *head = NULL;
    while (!feof(file)) {
        Post *newPost = malloc(sizeof(Post));
        if (fscanf(file, "%d|%d|%[^|]|%[^|]|%[^|]|%d\n", &newPost->id, &newPost->userId, newPost->title, newPost->content, newPost->type, &newPost->likes) != 6) {
            free(newPost); break;
        }
        newPost->next = head;
        head = newPost;
    }
    fclose(file);
    return head;
}

void freeUsers(User *head) { while (head) { User *tmp = head; head = head->next; free(tmp); } }
void freePosts(Post *head) { while (head) { Post *tmp = head; head = head->next; free(tmp); } }
